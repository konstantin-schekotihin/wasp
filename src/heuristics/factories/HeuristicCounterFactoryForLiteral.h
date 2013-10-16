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
 * File:   HeuristicCounterFactoryForLiteral.h
 * Author: Carmine Dodaro
 *
 * Created on 02 September 2013, 12.42
 */

#ifndef HEURISTICCOUNTERFACTORYFORLITERAL_H
#define	HEURISTICCOUNTERFACTORYFORLITERAL_H

#include <cassert>
class HeuristicCounterForLiteral;

class HeuristicCounterFactoryForLiteral
{
    public:
        virtual ~HeuristicCounterFactoryForLiteral() {}
        virtual HeuristicCounterForLiteral* createHeuristicCounter() = 0;        
};

#endif	/* HEURISTICCOUNTERFACTORYFORLITERAL_H */
