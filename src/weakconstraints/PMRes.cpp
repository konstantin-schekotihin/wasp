/*
 *
 *  Copyright 2013 Mario Alviano, Carmine Dodaro, and Francesco Ricca.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include "PMRes.h"

unsigned int
PMRes::run()
{    
    return solver.isWeighted() ? runWeighted() : runUnweighted();
}

unsigned int
PMRes::runUnweighted()
{
    trace_msg( weakconstraints, 1, "Starting algorithm PMRes" );
    solver.setComputeUnsatCores( true );
    solver.turnOffSimplifications();        
    initInUnsatCore();
    computeAssumptionsAND();
    unsigned int result = solver.solve( assumptionsAND, assumptionsOR );
    
    while( result == INCOHERENT )
    {        
        foundUnsat();
        result = solver.solve( assumptionsAND, assumptionsOR );
    }

    unsigned int cost = solver.computeCostOfModel();
    solver.printAnswerSet();
    solver.printOptimizationValue( cost );

    assert_msg( lb == cost, lb << " != " << cost );    
    return OPTIMUM_FOUND;
}

unsigned int
PMRes::runWeighted()
{
    statistics( &solver, disable() );
    trace_msg( weakconstraints, 1, "Starting algorithm OLL" );    
    
    preprocessingWeights();
    changeWeight( ub );
    computeAssumptionsANDStratified();        
    
    initInUnsatCore();    

    solver.setComputeUnsatCores( true );
    solver.turnOffSimplifications();

    while( true )
    {
        if( solver.solve( assumptionsAND, assumptionsOR ) != INCOHERENT )
        {            
            ub = solver.computeCostOfModel();
            solver.printAnswerSet();
            solver.printOptimizationValue( ub );
            solver.unrollToZero();
            solver.clearConflictStatus();
            if( !changeWeight( ub ) )
                break;
            assumptionsAND.clear();
            computeAssumptionsANDStratified();
        }
        else
        {
            if( !foundUnsat() )
                return INCOHERENT;
            assumptionsAND.clear();
            computeAssumptionsANDStratified();
        }
    }

    statistics( &solver, enable() );
    statistics( &solver, endSolving() );    
    assert_msg( lb == ub, lb << " != " << ub );    
    
    return OPTIMUM_FOUND;
}


bool
PMRes::addAuxClauses(
    vector< Literal >& optLiterals )
{
    assert( !optLiterals.empty() );
    trace_msg( weakconstraints, 2, "Adding aux clauses for " << optLiterals.size() << " elements" );
    for( unsigned int i = 0; i < optLiterals.size() - 1; i++ )
    {
        Literal lit = optLiterals[ i ];
        assert_msg( solver.isUndefined( lit ), "Lit " << lit << " is " << ( solver.isTrue( lit ) ? "true" : "false" ) );
        trace_msg( weakconstraints, 3, "Considering literal " << lit );
                
        Var aux = addAuxVariable();
        Clause* clause = new Clause();
        clause->addLiteral( lit );
        clause->addLiteral( Literal( aux, NEGATIVE ) );
        Var relaxVar = relaxClause( clause );

        solver.addOptimizationLiteral( Literal( relaxVar, POSITIVE ), 1, UINT_MAX, true );
        trace_msg( weakconstraints, 4, "Adding clause " << *clause );
        if( !addClauseToSolver( clause ) )
            return false;

        Clause* auxClause = new Clause();
        auxClause->addLiteral( Literal( aux, NEGATIVE ) );
        for( unsigned int j = i + 1; j < optLiterals.size(); j++ )
        {
            auxClause->addLiteral( optLiterals[ j ].getOppositeLiteral() );
            Clause* c1 = new Clause( 2 );
            c1->addLiteral( Literal( aux, POSITIVE ) );
            c1->addLiteral( optLiterals[ j ] );
            trace_msg( weakconstraints, 4, "Adding clause " << *c1 );
            if( !addClauseToSolver( c1 ) )
            {
                delete auxClause;
                return false;
            }
        }
        
        trace_msg( weakconstraints, 4, "Adding clause " << *auxClause );
        if( !addClauseToSolver( auxClause ) )
            return false;
    }        

    return true;
}

bool
PMRes::foundUnsat()
{
    ++numberOfCalls;
    assert( solver.getUnsatCore() != NULL );
    const Clause& unsatCore = *( solver.getUnsatCore() );

    //The incoherence does not depend on weak constraints
    if( unsatCore.size() == 0 )
        return false;

    solver.clearConflictStatus();
    solver.unrollToZero();

    for( unsigned int i = 0; i < unsatCore.size(); i++ )
    {
        Var v = unsatCore[ i ].getVariable();
        visit( v );
    }        
    unsigned int minWeight = computeMinWeight();
    lb += minWeight;
    solver.foundLowerBound( lb );
    vector< Literal > optLiterals;

    bool trivial = false;
    Clause* clause = new Clause();
    trace_msg( weakconstraints, 1, "Computing hard clause " );
    unsigned int originalSize = solver.numberOfOptimizationLiterals();

    unsigned int atLevelZeroWeight = 0;
    for( unsigned int i = 0; i < originalSize; i++ )
    {
        OptimizationLiteralData& data = solver.getOptimizationLiteral( i );
        Literal lit = data.lit;
        if( data.isRemoved() || !visited( lit.getVariable() ) )
            continue;

        trace_msg( weakconstraints, 2, "Considering literal " << lit );
        trace_msg( weakconstraints, 3, "which is " << ( visited( lit.getVariable() ) ? "in the core" : "not in the core: skipped" ) );
        trace_msg( weakconstraints, 3, "its decision level is " << solver.getDecisionLevel( lit ) );
        assert_msg( !solver.isFalse( lit ), "Lit " << lit << " is false" );
        trace_msg( weakconstraints, 3, "and it is " << ( solver.isUndefined( lit ) ? "undefined" : "true" ) );
        trace_msg( weakconstraints, 3, "its weight is " << data.weight );

        if( solver.isTrue( lit ) )
            trivial = true;

        data.remove();
        if( solver.getDecisionLevel( lit ) > 0 )
        {
            clause->addLiteral( lit );
            optLiterals.push_back( lit.getOppositeLiteral() );

            if( data.weight > minWeight )
                solver.addOptimizationLiteral( lit, data.weight - minWeight, UINT_MAX, true );
        }
        else
        {
            atLevelZeroWeight += data.weight;
        }
    }        

    if( clause->size() != 0 )
    {
        if( !trivial )
        {
            trace_msg( weakconstraints, 1, "Adding clause " << *clause );            
            addClauseToSolver( clause );
        }
        #ifndef NDEBUG
        bool res =
        #endif
        addAuxClausesCompressed( optLiterals, minWeight );
        assert( res );
    }
    else
    {
        trace_msg( weakconstraints, 1, "Derived empty clause" );
        if( atLevelZeroWeight > minWeight )
            lb += atLevelZeroWeight - minWeight;
    }

    assumptionsAND.clear();
    computeAssumptionsAND();    
    return true;
}

bool
PMRes::addAuxClausesCompressed(
    vector< Literal >& optLiterals,
    unsigned int minWeight )
{
    assert( !optLiterals.empty() );
    vector< Literal > auxLits;
    auxLits.reserve( optLiterals.size() );

    auxLits.push_back( Literal::null );
    for( unsigned int i = 1; i < optLiterals.size() - 1; i++ )
        auxLits.push_back( Literal( addAuxVariable(), POSITIVE ) );    
    auxLits.push_back( optLiterals[ optLiterals.size() - 1 ].getOppositeLiteral() );    
    
    assert( !optLiterals.empty() );
    trace_msg( weakconstraints, 2, "Adding aux clauses for " << optLiterals.size() << " elements" );
    for( unsigned int i = 0; i < optLiterals.size() - 1; i++ )
    {
        Literal lit = optLiterals[ i ];
        assert_msg( solver.isUndefined( lit ), "Lit " << lit << " is " << ( solver.isTrue( lit ) ? "true" : "false" ) );
        trace_msg( weakconstraints, 3, "Considering literal " << lit );
        
        Clause* clause = new Clause();
        clause->addLiteral( lit );
        clause->addLiteral( auxLits[ i + 1 ].getOppositeLiteral() );
        Var relaxVar = relaxClause( clause );

        solver.addOptimizationLiteral( Literal( relaxVar, POSITIVE ), minWeight, UINT_MAX, true );
        trace_msg( weakconstraints, 4, "Adding clause1 " << *clause );
        if( !addClauseToSolver( clause ) )
            return false;

        if( i + 2 < optLiterals.size() )
        {
            Clause* auxClause = new Clause();
            auxClause->addLiteral( auxLits[ i + 1 ].getOppositeLiteral() );
            auxClause->addLiteral( optLiterals[ i + 1 ].getOppositeLiteral() );
            auxClause->addLiteral( auxLits[ i + 2 ] );
                
            Clause* c1 = new Clause( 2 );
            c1->addLiteral( auxLits[ i + 1 ] );
            c1->addLiteral( optLiterals[ i + 1 ] );
            trace_msg( weakconstraints, 4, "Adding clause2 " << *c1 );
            if( !addClauseToSolver( c1 ) )
            {
                delete auxClause;
                return false;
            }        
        
            Clause* c2 = new Clause( 2 );
            c2->addLiteral( auxLits[ i + 1 ] );
            c2->addLiteral( auxLits[ i + 2 ].getOppositeLiteral() );
            trace_msg( weakconstraints, 4, "Adding clause3 " << *c2 );
            if( !addClauseToSolver( c2 ) )
            {
                delete auxClause;
                return false;
            }
            trace_msg( weakconstraints, 4, "Adding clause4 " << *auxClause );
            if( !addClauseToSolver( auxClause ) )
                return false;
        }        
    }        

    return true;
}

//bool
//PMRes::addClauseToSolverAndCheckDuplicatesAndTautological(
//    Clause* clause )
//{
//    if( clause->removeDuplicatesAndCheckIfTautological() )
//    {
//        delete clause;
//        return true;
//    }    
//    return addClauseToSolver( clause );
//}

bool
PMRes::addClauseToSolver(
    Clause* clause )
{
    return solver.addClauseRuntime( clause );
//    unsigned int size = clausePointer->size();
//    Clause& clause = *clausePointer;    
//    bool res;
//    switch( size )
//    {
//        case 0:
//            res = false;
//            delete clausePointer;
//            break;
//
//        case 1:
//            res = solver.addClause( clause[ 0 ] );
//            delete clausePointer;
//            break;
//
//        case 2:
//            res = solver.addClause( clause[ 0 ], clause[ 1 ] );
//            delete clausePointer;
//            break;
//
//        default:
//            res = solver.addClause( clausePointer );
//            if( res )
//                solver.attachClause( clause );
//            break;
//    }
//
//    return res;
}

Var
PMRes::relaxClause(
    Clause* clausePointer )
{
    Clause& clause = *clausePointer;
    Var aux = addAuxVariable();
//    for( unsigned int i = 0; i < clause.size(); i++ )
//    {
//        Clause* c1 = new Clause( 2 );
//        c1->addLiteral( Literal( aux, NEGATIVE ) );
//        c1->addLiteral( clause[ i ] );
//        trace_msg( weakconstraints, 4, "Adding clause " << *c1 );
//        if( !addClauseToSolver( c1 ) )
//            return false;        
//    }
    
    clause.addLiteral( Literal( aux, POSITIVE ) );    
    return aux;
}