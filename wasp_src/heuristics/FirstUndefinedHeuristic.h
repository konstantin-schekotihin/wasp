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
 * File:   FirstUndefinedHeuristic.h
 * Author: Carmine Dodaro
 *
 * Created on 05 September 2013, 12.39
 */

#ifndef FIRSTUNDEFINEDHEURISTIC_H
#define	FIRSTUNDEFINEDHEURISTIC_H

#include "DecisionHeuristic.h"

class FirstUndefinedHeuristic : public DecisionHeuristic
{
    public:
        inline FirstUndefinedHeuristic();
        virtual Literal makeAChoice( Solver& solver );
        virtual void onLearning( Solver& solver );
        virtual void onRestarting( Solver& solver );
};

FirstUndefinedHeuristic::FirstUndefinedHeuristic() : DecisionHeuristic()
{
}

#endif	/* FIRSTUNDEFINEDHEURISTIC_H */

