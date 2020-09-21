//
//  rule_set.cpp
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

#include "rule_set.h"

#include "CheckUtil.h"

using std::string;
using std::vector;

fuzzy::RuleSet::RuleSet()
    : _name("")
    , _implication(Implication::LARSEN)
{
    // nop
}

fuzzy::RuleSet::RuleSet(string name, Implication implication)
    : _name(name)
    , _implication(implication)
{
    // nop
}

void fuzzy::RuleSet::populate_mu_map(const std::vector<double> inputValues, mu_map& _consequents_to_mus) const
{
    _consequents_to_mus.clear();

    // Fire each rule to determine the µ value (degree of fit).
    for(auto& rule: _rules)
    {
        double mu = rule.fire(inputValues);

        auto iter = _consequents_to_mus.find(rule.consequent());
        if(iter == end(_consequents_to_mus))
        {
            // Didn't find
            _consequents_to_mus.insert(std::make_pair(rule.consequent(), mu));
        }
        else if(mu > iter->second)
        {
            // Did find, and latest µ is superior to the previous best
            iter->second = mu; // keep the max mu
        }
    }
}

double fuzzy::RuleSet::scale_and_defuzzify(const mu_map& _consequents_to_mus) const
{
    double numerator   = 0;
    double denominator = 0;

    // This isn't DRY but it's fastest this way.
    if(_implication == Implication::MAMDANI)
    {
        for(const auto& entry: _consequents_to_mus)
        {
            Trapezoid tmp{(entry.first).mamdami(entry.second)};
            numerator += (tmp.calculateXCentroid() * tmp.height());
            denominator += tmp.height();
        }
    }
    else
    {
        for(const auto& entry: _consequents_to_mus)
        {
            Trapezoid tmp{(entry.first).larsen(entry.second)};
            numerator += (tmp.calculateXCentroid() * tmp.height());
            denominator += tmp.height();
        }
    }

    if(denominator == 0)
    {
        return 0;
    }

    return numerator / denominator;
}

double fuzzy::RuleSet::calculate(const double value) const
{
    return calculate(std::vector<double>{value});
}

double fuzzy::RuleSet::calculate(const vector<double> inputValues) const
{
    __ENTER_FUNCTION
    fuzzy::RuleSet::mu_map _consequents_to_mus;
    populate_mu_map(inputValues, _consequents_to_mus);
    return scale_and_defuzzify(_consequents_to_mus);
    __LEAVE_FUNCTION
    return 0.0f;
}
