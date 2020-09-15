#ifndef RULE_H
#define RULE_H
//
//  rule.h
//  fam
//
//  Created by Chris Powell on 8/19/13.
//  Copyright (c) 2013 Prylis Inc. All rights reserved.
//
//  This file is part of The C++ Fuzzy Associative Memory
//  http://github.com/cpowell/fuzzy-associative-memory-cplusplus
//  You can redistribute and/or modify this software only in accordance with
//  the terms found in the "LICENSE" file included with the library.
//

#include <string>
#include <vector>

#include "trapezoid.h"

namespace fuzzy
{
    enum class Conjunction : char
    {
        AND,
        OR
    };

    /**
     A rule is basically a statement of "If this then that."
     The antecedents are the "if this" part, and the consequent is the "that" part.
     Given the antecedents, and some input, the rule can "fire" to determine
     a "degree of fit" - i.e. how much this rule applies to the given circumstances.
     */
    class Rule
    {
    private:
        // A rule has one or more antecedent FuzzySets
        std::vector<Trapezoid> _antecedents;

        // The conjunction joins the antecedents together
        Conjunction _conjunction;

        // When a rule fires, it returns the degree-of-fit of this consequent
        // If you know another object is going to outlive you and you want to observe it, use a (non-owning) raw
        // pointer.
        Trapezoid _consequent;

        // This is just a human-readable summary of what this rule does
        std::string _naturalLanguage;

    public:
        /**
         Construct a rule.
         The conjunction is always required, but is ignored for single-element antecedents.
         @param antecedents a vector of one or more FuzzySets
         @param conjunction how the antecedents work together ("and" or "or")
         @param consequent a FuzzySet that is the result of this Rule
         @param naturalLanguage an optional human-readable summary of this rule
         */
        Rule(const std::vector<Trapezoid>& antecedents,
             const Conjunction&            conjunction,
             const Trapezoid&              consequent,
             std::string                   naturalLanguage = "");

        /**
         Retrieve this rule's consequent.
         */
        const Trapezoid& consequent() const { return _consequent; }

        /**
         Retrieve the rule's natural-language description.
         @return the description in string form
         */
        std::string name() const { return _naturalLanguage; }

        /**
         Given the provided input values, calculate the rule's degree of fit (µ).
         This means: it goes through the antecedent fuzzy-sets, calculates the µ value of each,
         and computes an aggregate µ value according to the rule's conjunction.
         The size of 'values' must equal the size of the antecedents vector.
         @param values a vector of input values, one for each antecedent
         @return this rule's degree-of-fit (µ) as a range of 0 (no fit) to 1 (perfect fit)
         */
        double fire(const std::vector<double>& values) const;
    };
} // namespace fuzzy

#endif /* RULE_H */
