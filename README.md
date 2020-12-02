ndf
===

A tiny prototype to build Disjunctive Normal Form

References
----------
https://en.wikipedia.org/wiki/Disjunctive_normal_form
https://proofwiki.org/wiki/Rule_of_Distribution/Conjunction_Distributes_over_Disjunction/Left_Distributive/Formulation_2
https://stackoverflow.com/questions/17238768/distributing-and-over-or-in-a-binary-tree-conjunctive-normal-form


Internals
---------

Source, result and intermediate data represented as (nonbinary) tree.
Nodes may be of 'or', 'and' or any other type, presumable literal.
