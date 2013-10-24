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
 
#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "util/Assert.h"
#include "Literal.h"

class Heuristic
{
public:
    typedef double VariableData;
    typedef double ClauseData;
    
    virtual ~Heuristic() {}
    virtual VariableData createVariableData() { return 0.0; }
    virtual ClauseData createClauseData() { return 0.0; }

    virtual Literal makeAChoice() = 0;
    
    virtual void onNewVariable( Variable& variable ) = 0;
    virtual void onRestart() = 0;
    virtual void onConflict() = 0;
    virtual void onLearning( Clause* clause ) = 0;
    virtual void onLiteralInvolvedInConflict( Literal literal ) = 0;
    virtual void deleteClauses() = 0;
    virtual bool hasToRestart() = 0;
};

#endif
