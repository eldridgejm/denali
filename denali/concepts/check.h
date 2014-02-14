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
