#include <string_view>
#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <algorithm>

// Normal Disjunction Form builder

using String = std::string;

class ASTFunction;

using ASTPtr = std::shared_ptr<ASTFunction>;
using ASTs = std::vector<ASTPtr>;

struct ASTFunction
{
    String name;
    String id;
    ASTPtr arguments;
    ASTs children;
    template <typename STREAM>
    void dumpTree(STREAM & ostr, size_t indent = 0) const;
    void normTree();
    String getID() const {return name + ':' + id;}
    bool operator==(const ASTFunction& rhs) {return (void*)this == (void*)&rhs;}

    bool isOr() const {return name == "or";}
    bool isAnd() const {return name == "and";}

    ASTFunction(std::string_view name_, std::string_view id_ = "")
        :name(name_), id(id_)
    {}
};

template <typename STREAM>
void ASTFunction::dumpTree(STREAM & ostr, size_t indent) const
{
    String indent_str(indent, '-');
    ostr << indent_str << getID() << ", " << (void*)this << std::endl;
    for (const auto & child : children)
    {
        child->dumpTree(ostr, indent + 1);
    }
}

// Distribute Or over And
// https://proofwiki.org/wiki/Rule_of_Distribution/Conjunction_Distributes_over_Disjunction/Left_Distributive/Formulation_2
ASTPtr distribute(ASTPtr node)
{
    ASTPtr or_child;

    if (node->isAnd())
    {
        auto or_child = std::find_if(node->children.begin(), node->children.end(), [](auto & arg)
            {
                return arg->isOr();
            });
        if (or_child == node->children.end())
        {
            return node;
        }

        auto rest = std::make_shared<ASTFunction>("and");
        for (auto & arg : node->children)
        {
            if (*arg == **or_child)
            {
                continue;
            }
            rest->children.push_back(arg);
        }
        if (rest->children.size() == 1)
        {
            rest = rest->children[0];
        }

        ASTs lst;
        for (auto & arg : (*or_child)->children)
        {
            auto and_node = std::make_shared<ASTFunction>("and");
            and_node->children.push_back(arg);
            and_node->children.push_back(rest);

            lst.push_back(distribute(and_node));
        }
        if (lst.size() == 1)
        {
            return lst[0];
        }
        else
        {
            auto ret = std::make_shared<ASTFunction>("or");
            ret->children = lst;
            return ret;
        }

    }
    else if (node->isOr())
    {
        ASTs lst;
        for (auto & arg : node->children)
        {
            lst.push_back(distribute(arg));
        }

        if (lst.size() == 1)
        {
            return lst[0];
        }
        else
        {
            auto ret = std::make_shared<ASTFunction>("or");
            ret->children = lst;
            return ret;
        }

    }
    else
    {
        return node;
    }
}


void ASTFunction::normTree()
{
    bool only_or = true;
    bool only_and = true;

    for (const auto & child : children)
    {
        if (child->isOr())
        {
            only_and = false;
        }
        if (child->isAnd())
        {
            only_or = false;
        }
    }

    if ((only_or && isOr() || only_and && isAnd()) && children.size() > 1)
    {
        ASTs new_children;
        for (const auto & child : children)
        {
            if(child->isOr() || child->isAnd())
            {
                std::copy(child->children.begin(), child->children.end(), std::back_inserter(new_children));
            }
            else
            {
                new_children.push_back(child);
            }
        }
        children = new_children;
    }

    for (const auto & child : children)
    {
        child->normTree();
    }
}

ASTPtr f1()
{
    auto node1 = std::make_shared<ASTFunction>("or");
    auto node2 = std::make_shared<ASTFunction>("and");
    auto node21 = std::make_shared<ASTFunction>("literal", "two_1");
    auto node22 = std::make_shared<ASTFunction>("literal", "two_2");
    node2->children.push_back(node21);
    node2->children.push_back(node22);
    auto node3 = std::make_shared<ASTFunction>("and");
    auto node31 = std::make_shared<ASTFunction>("literal", "three_1");
    auto node32 = std::make_shared<ASTFunction>("literal", "three_2");
    node3->children.push_back(node31);
    node3->children.push_back(node32);

    node1->children.push_back(node2);
    node1->children.push_back(node3);

    return node1;
}

ASTPtr f2()
{
    auto node1 = std::make_shared<ASTFunction>("and", "one");
    auto node2 = std::make_shared<ASTFunction>("or", "two");
    auto node21 = std::make_shared<ASTFunction>("literal", "two_1");
    auto node22 = std::make_shared<ASTFunction>("literal", "two_2");
    node2->children.push_back(node21);
    node2->children.push_back(node22);
    auto node3 = std::make_shared<ASTFunction>("and", "three");
    auto node31 = std::make_shared<ASTFunction>("literal", "three_1");
    auto node32 = std::make_shared<ASTFunction>("literal", "three_2");
    node3->children.push_back(node31);
    node3->children.push_back(node32);

    node1->children.push_back(node2);
    node1->children.push_back(node3);

    return node1;
}

ASTPtr f3()
{
    auto node1 = std::make_shared<ASTFunction>("and");
    auto node2 = std::make_shared<ASTFunction>("or");
    auto node21 = std::make_shared<ASTFunction>("literal", "two_1");
    auto node22 = std::make_shared<ASTFunction>("literal", "two_2");
    node2->children.push_back(node21);
    node2->children.push_back(node22);
    auto node3 = std::make_shared<ASTFunction>("literal", "three");

    node1->children.push_back(node2);
    node1->children.push_back(node3);

    return node1;
}

ASTPtr f4()
{
    auto node1 = std::make_shared<ASTFunction>("and");
    auto node2 = std::make_shared<ASTFunction>("or");
    auto node21 = std::make_shared<ASTFunction>("literal", "two_1");
    auto node22 = std::make_shared<ASTFunction>("literal", "two_2");
    node2->children.push_back(node21);
    node2->children.push_back(node22);
    auto node31 = std::make_shared<ASTFunction>("literal", "three_1");
    auto node32 = std::make_shared<ASTFunction>("literal", "three_2");
    auto node33 = std::make_shared<ASTFunction>("literal", "three_3");

    node1->children.push_back(node2);
    node1->children.push_back(node31);
    node1->children.push_back(node32);
    node1->children.push_back(node33);

    return node1;
}

ASTPtr f5()
{
    auto node1 = std::make_shared<ASTFunction>("and");
    auto node2 = std::make_shared<ASTFunction>("or");
    auto node21 = std::make_shared<ASTFunction>("literal", "two_1");
    auto node22 = std::make_shared<ASTFunction>("literal", "two_2");
    node2->children.push_back(node21);
    node2->children.push_back(node22);
    auto node3 = std::make_shared<ASTFunction>("or");
    auto node31 = std::make_shared<ASTFunction>("literal", "three_1");
    auto node32 = std::make_shared<ASTFunction>("literal", "three_2");
    node3->children.push_back(node31);
    node3->children.push_back(node32);

    node1->children.push_back(node2);
    node1->children.push_back(node3);

    return node1;
}

int main(int argc, char** argv)
{
    for (auto f : {f1, f2, f3, f4, f5})
    {
        auto root = f();
        std::cout << std::endl << "Original ===========================" << std::endl;
        root->dumpTree(std::cout);
        auto distributed = distribute(root);
        std::cout << "Distributed ===========================" << std::endl;
        distributed->dumpTree(std::cout);
        distributed->normTree();
        std::cout << "Normalized ===========================" << std::endl;
        distributed->dumpTree(std::cout);
    }
}
