/*
 *
 *  Copyright 2013 Mario Alviano, Carmine Dodaro, Wolfgang Faber, Nicola Leone, Francesco Ricca, and Marco Sirianni.
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

/* 
 * File:   Solver.h
 * Author: Carmine Dodaro
 *
 * Created on 21 July 2013, 17.36
 */

#ifndef SOLVER_H
#define	SOLVER_H

#include <cassert>
#include <vector>
#include <list>
using namespace std;

#include "../Clause.h"
#include "../LearnedClause.h"
#include "../Variable.h"
#include "../Literal.h"
#include "../WaspRule.h"
#include "../stl/List.h"
#include "../stl/UnorderedSet.h"
#include "../learning/AggressiveDeletionStrategy.h"
#include "../learning/DeletionStrategy.h"
#include "../learning/FirstUIPLearningStrategy.h"
#include "../learning/LearningStrategy.h"
#include "../learning/SequenceBasedRestartsStrategy.h"
#include "../heuristics/BerkminHeuristic.h"
#include "../heuristics/DecisionHeuristic.h"
#include "../heuristics/factories/HeuristicCounterFactoryForLiteral.h"
#include "../heuristics/factories/BerkminCounterFactory.h"
#include "../outputBuilders/OutputBuilder.h"
#include "../outputBuilders/DimacsOutputBuilder.h"
#include "../outputBuilders/WaspOutputBuilder.h"
#include "../heuristics/FirstUndefinedHeuristic.h"
#include "../learning/RestartsBasedDeletionStrategy.h"
#include "../learning/GeometricRestartsStrategy.h"

class Solver
{
    public:
        inline Solver();
        ~Solver();
        
        virtual void init() = 0;
        virtual bool preprocessing();
        virtual bool solve();
        virtual void propagate( Literal literalToPropagate ) = 0;
        
        void addVariable( const string& name );
        void addVariable();
        
//        AuxLiteral* addAuxVariable();
//        inline bool existsAuxLiteral( unsigned int id ) const;
//        inline AuxLiteral* getAuxLiteral( unsigned int id );
        
        inline void addClause( Clause* clause );
        inline void addLearnedClause( LearnedClause* learnedClause );        
        
        Literal getLiteral( int lit );
        inline void addTrueLiteral( int lit );
        
        inline Literal getNextLiteralToPropagate();
        inline bool hasNextLiteralToPropagate() const;        
        
        inline unsigned int getCurrentDecisionLevel();
        inline void incrementCurrentDecisionLevel();
        
        void onLiteralAssigned( Literal literal, Clause* implicant );        
        
        void decreaseLearnedClausesActivity();
        void onDeletingLearnedClausesThresholdBased();
        void onDeletingLearnedClausesAvgBased();
        inline void onLearningClause( Literal literalToPropagate, LearnedClause* learnedClause, unsigned int backjumpingLevel );
        inline void onLearningUnaryClause( Literal literalToPropagate, LearnedClause* learnedClause );        
        inline void onRestarting();        
        
        inline unsigned int numberOfClauses();
        inline unsigned int numberOfLearnedClauses();        
        inline unsigned int numberOfAssignedLiterals();
        inline unsigned int numberOfVariables();
        inline unsigned int numberOfAuxVariables();
        
        inline const UnorderedSet< Variable* >& getUndefinedVariables();
        inline const List< LearnedClause* >& getLearnedClauses();
        
        inline void setAChoice( Literal choice );        
        
        inline void analyzeConflict();
        inline void clearConflictStatus();
        inline void chooseLiteral();
        inline bool conflictDetected();
        inline void foundIncoherence();
        inline bool hasUndefinedLiterals();
        inline void printAnswerSet();
        
        void unroll( unsigned int level );
        inline void unrollOne();
        
        void printProgram()
        {
            for( list< Clause* >::iterator it = clauses.begin(); it != clauses.end(); ++it )
            {
                cout << *( *it ) << endl;
            }
        }
        
    private:
        Solver( const Solver& )
        {
            assert( "The copy constructor has been disabled." && 0 );
        }
                
        void addVariableInternal( Variable* variable );
        inline void deleteLearnedClause( LearnedClause* learnedClause, List< LearnedClause* >::iterator iterator );
        
        list< Literal > trueLiterals;        
        list< Literal > literalsToPropagate;
        
        unsigned int currentDecisionLevel;
        
        List< Variable* > assignedVariables;
        
        /* Data structures */
        vector< Variable* > variables;
        
        List< Clause* > clauses;
        List< LearnedClause* > learnedClauses;
        
        bool conflict;
        vector< unsigned int > unrollVector;
        
        Literal conflictLiteral;
        Clause* conflictClause;
        
        DecisionHeuristic* decisionHeuristic;
        HeuristicCounterFactoryForLiteral* heuristicCounterFactoryForLiteral;
        
        LearningStrategy* learningStrategy;
        DeletionStrategy* deletionStrategy;
        
        OutputBuilder* outputBuilder;
        
        UnorderedSet< Variable* > undefinedVariables;        
};

Solver::Solver() : currentDecisionLevel( 0 ), conflict( false ), conflictLiteral( NULL ), conflictClause( NULL )
{
    //Add a fake position.
    variables.push_back( NULL );
//    learningStrategy = new FirstUIPLearningStrategy( new SequenceBasedRestartsStrategy() );
    learningStrategy = new FirstUIPLearningStrategy( new SequenceBasedRestartsStrategy( 100000 ) );
//    deletionStrategy = new AggressiveDeletionStrategy();
    deletionStrategy = new RestartsBasedDeletionStrategy();
    
    heuristicCounterFactoryForLiteral = new BerkminCounterFactory();
    decisionHeuristic = new BerkminHeuristic();
    
    outputBuilder = new DimacsOutputBuilder();    
//    outputBuilder = new WaspOutputBuilder();
}

void
Solver::addClause(
    Clause* clause )
{
    clauses.push_back( clause );
}

void
Solver::addLearnedClause( 
    LearnedClause* learnedClause )
{    
    learnedClauses.push_back( learnedClause );
}

void
Solver::addTrueLiteral(
    int lit )
{
    Literal literal = getLiteral( lit );
    trueLiterals.push_back( literal );
}

Literal
Solver::getNextLiteralToPropagate()
{
    assert( !literalsToPropagate.empty() );
    Literal tmp = literalsToPropagate.back();
    literalsToPropagate.pop_back();
    return tmp;
}
        
bool
Solver::hasNextLiteralToPropagate() const
{
    return !literalsToPropagate.empty();
}

unsigned int
Solver::getCurrentDecisionLevel()
{
    return currentDecisionLevel;
}

void
Solver::incrementCurrentDecisionLevel()
{
    currentDecisionLevel++;
    unrollVector.push_back( assignedVariables.size() );
    
    assert( currentDecisionLevel == unrollVector.size() );
}

void
Solver::unrollOne()
{
    unroll( currentDecisionLevel - 1 );
}

void
Solver::onLearningClause( 
    Literal literalToPropagate, 
    LearnedClause* learnedClause, 
    unsigned int backjumpingLevel )
{
    assert( "Backjumping level is not valid." && backjumpingLevel < currentDecisionLevel );
    unroll( backjumpingLevel );    
    
    assert( "Each learned clause has to be an asserting clause." && literalToPropagate != NULL );
    assert( "Learned clause has not been calculated." && learnedClause != NULL );
    
    Clause* clause = static_cast< Clause* >( learnedClause );    
    onLiteralAssigned( literalToPropagate, clause );
    
    deletionStrategy->onLearning( *this, learnedClause );
}

void
Solver::onLearningUnaryClause(
    Literal literalToPropagate,
    LearnedClause* learnedClause )
{
    onRestarting();
    onLiteralAssigned( literalToPropagate, NULL );

    assert( "Learned clause has not been calculated." && learnedClause != NULL );    
    delete learnedClause;
}

void
Solver::onRestarting()
{
    deletionStrategy->onRestarting();
    decisionHeuristic->onRestarting( *this );
    unroll( 0 );
}

void
Solver::deleteLearnedClause( 
    LearnedClause* learnedClause,
    List< LearnedClause* >::iterator iterator )
{
    learnedClause->detachClause();
    
    delete learnedClause;    
    learnedClauses.erase( iterator );
}

unsigned int
Solver::numberOfClauses()
{
    return clauses.size();
}

unsigned int
Solver::numberOfLearnedClauses()
{
    return learnedClauses.size();
}

const UnorderedSet< Variable* >&
Solver::getUndefinedVariables()
{
    return undefinedVariables;
}

const List< LearnedClause* >&
Solver::getLearnedClauses()
{
    return learnedClauses;
}

bool
Solver::conflictDetected()
{
    return conflict;
}

bool
Solver::hasUndefinedLiterals()
{
    return !undefinedVariables.empty();
}

void
Solver::printAnswerSet()
{
    outputBuilder->startModel();    
    for( List< Variable* >::iterator it = assignedVariables.begin(); it != assignedVariables.end(); ++it )
    {
        if( !( *it )->isHidden() )
            outputBuilder->printVariable( *it );
    }
    outputBuilder->endModel();
}

void
Solver::foundIncoherence()
{
    outputBuilder->onProgramIncoherent();
}

void
Solver::chooseLiteral()
{
    Literal choice = decisionHeuristic->makeAChoice( *this );    
    setAChoice( choice );    
}

void
Solver::analyzeConflict()
{    
    conflictLiteral.setOrderInThePropagation( numberOfAssignedLiterals() + 1 );
    learningStrategy->onConflict( conflictLiteral, conflictClause, *this );
    decisionHeuristic->onLearning( *this );
    clearConflictStatus();
}

void
Solver::clearConflictStatus()
{
    conflict = false;
    conflictLiteral = NULL;
    conflictClause = NULL;
}

unsigned int
Solver::numberOfAssignedLiterals()
{
    return assignedVariables.size();
}

unsigned int
Solver::numberOfVariables()
{
    return variables.size();
}

void
Solver::setAChoice( 
    Literal choice )
{
    assert( choice != NULL );
    incrementCurrentDecisionLevel();
    assert( choice.isUndefined() );
    onLiteralAssigned( choice, NULL );
}

//bool
//Solver::existsAuxLiteral(
//    unsigned int id ) const
//{
//    return( id < auxLiterals.size() );
//}
//
//AuxLiteral*
//Solver::getAuxLiteral(
//    unsigned int id )
//{
//    assert( existsAuxLiteral( id ) );
//    return auxLiterals[ id ];
//}

//unsigned int
//Solver::numberOfAuxVariables()
//{
//    return auxLiterals.size();
//}

#endif	/* SOLVER_H */

