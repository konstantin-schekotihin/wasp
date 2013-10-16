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

#include "OutputBuilder.h"

//void
//OutputBuilder::onAnswerSetFoundWithWeakConstraint(
//    Interpretation&,
//    Program& program)
//{
//    assert( program.hasWeakConstraints() );
//    Aggregate* aggr = program.getAggregateRepresentingWeakConstraints();
//    assert( aggr != NULL );
//    vector< int >& levels = program.getLevelsOfWeakConstraints();
//    if( !levels.empty() )
//    {
//        cout << COST;
//        for( int i = levels.size() - 1; i >= 0; --i )
//        {
//            cout << " " << program.getCostOfLevel( i, aggr->getCurrentValue() ) << WEIGHT_LEVEL_WEAKCONSTRAINT_SEPARATOR << levels[ i ];               
//        }
//        #ifndef TRACE_OFF
//            cout << " = " << aggr->getCurrentValue();
//        #endif
//        cout << endl;
//    }
//}