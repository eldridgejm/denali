// Copyright (c) 2014, Justin Eldridge, Mikhail Belkin, and Yusu Wang
// at The Ohio State University. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef DENALI_CONCEPTS_CHECK_H
#define DENALI_CONCEPTS_CHECK_H

/// \file
/// \brief Contains functions for checking concepts.

namespace denali {
namespace concepts {

template <class T> inline void ignore_unused_variable_warning(const T&) {}

/// \brief Check that the particular Concept is satisfied by the Type.
template <typename Concept, typename Type>
void checkConcept()
{
    typedef typename Concept::template Constraints<Type> ConceptCheck;
    void (ConceptCheck::*x)() = &ConceptCheck::constraints;
    ignore_unused_variable_warning(x);
}


template <template<class> class Concept, template<class> class Type,
          typename Alpha>
void checkConcept()
{
    typedef typename Concept<Alpha>::
    template Constraints<Type, Alpha> ConceptCheck;
    void (ConceptCheck::*x)() = &ConceptCheck::constraints;
    ignore_unused_variable_warning(x);
}


template <template<class,class> class Concept, template<class,class> class Type,
          typename Alpha, typename Beta>
void checkConcept()
{
    typedef typename Concept<Alpha, Beta>::
    template Constraints<Type, Alpha, Beta> ConceptCheck;
    void (ConceptCheck::*x)() = &ConceptCheck::constraints;
    ignore_unused_variable_warning(x);
}


/// Check that a concept obeys its own constraints.
template <typename Concept>
void checkSelfConsistent()
{
    checkConcept<Concept,Concept>();
}
}
}

#endif
