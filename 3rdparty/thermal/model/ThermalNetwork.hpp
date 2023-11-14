#pragma once

#include "generic/math/MathUtility.hpp"
#include "generic/tools/Format.hpp"
#include "generic/circuit/MNA.hpp"
#include <unordered_map>
#include <memory>
#include <set>

namespace thermal {
namespace model {

template <typename num_type>
class ThermalNetwork
{
public:
    inline static constexpr num_type unknownT = std::numeric_limits<num_type>::max();
    struct Node
    {
        num_type t = unknownT;//unit: K
        num_type c = 0;
        num_type hf = 0;//unit: W
        num_type htc = 0;//unit: W/k
        std::vector<size_t> ns;
        std::vector<num_type> rs;

        std::string msg(size_t index) const
        {
            using namespace generic::fmt;
            std::stringstream ss;
            ss << Fmt2Str("ID: %1%, T:%2%, C:%3%, HF:%4%, HTC:%5%", index, t, c, hf, htc);
            if (not ns.empty()) {
                ss << ", N:[";
                for (size_t i = 0; i < ns.size(); ++i)
                    ss << Fmt2Str("%1%(%2%) ", ns.at(i), rs.at(i));
                ss << ']';
            }
            return ss.str();
        }
    };

    struct Edge
    {
        size_t x, y;
        num_type  r;//unit: K/W
    };

    explicit ThermalNetwork(size_t nodes)
    {
        m_nodes.assign(nodes, Node{});
    }

    virtual ~ThermalNetwork() = default;

    size_t Size() const
    {
        return m_nodes.size();
    }

    size_t Source() const
    {
        size_t size{0};
        for (const auto & node : m_nodes) {
            if (node.hf != 0 || node.htc != 0)
                size++;
        }
        return size;//todo , remove
    }

    Node & operator[] (size_t i) 
    {
        return m_nodes[i]; 
    }

    const Node & operator[] (size_t i) const
    {
        return m_nodes.at(i);
    }
    
    size_t AppendNode(num_type t = unknownT)
    {
        size_t index = Size();
        m_nodes.push_back(Node());
        m_nodes.back().t = t;
        return index;
    }

    const std::vector<size_t> & NS(size_t node) const
    {
        return m_nodes[node].ns;
    }

    void SetT(size_t node, num_type t)
    {
        m_nodes[node].t = t;
    }

    num_type GetT(size_t node) const
    {
        return m_nodes[node].t;
    }

    void SetHF(size_t node, num_type hf)
    {
        m_nodes[node].hf = hf;
    }

    num_type GetHF(size_t node) const
    {
        return m_nodes[node].hf;
    }

    void SetHTC(size_t node, num_type htc)
    {
        m_nodes[node].htc = htc;
    }

    num_type GetHTC(size_t node) const
    {
        return m_nodes[node].htc;
    }

    void SetC(size_t node, num_type c)
    {
        m_nodes[node].c = c;
    }

    void SetR(size_t node1, size_t node2, num_type r)
    {
        //todo eff
        for (size_t i = 0; i < m_nodes[node1].ns.size(); ++i) {
            if (m_nodes[node1].ns[i] == node2) {
                r = 1 / (1 / r + 1 / m_nodes[node1].rs[i]);
                m_nodes[node1].rs[i] = r;
                for (size_t j = 0; j < m_nodes[node2].ns.size(); ++j) {
                    if (m_nodes[node2].ns[j] == node1)
                        m_nodes[node2].rs[j] = r;
                }
                return;
            }
        }
        m_nodes[node1].ns.push_back(node2);
        m_nodes[node1].rs.push_back(r);

        m_nodes[node2].ns.push_back(node1);//wbtest remove
        m_nodes[node2].rs.push_back(r);//wbtest remove
    }

    std::vector<Node> & GetNodes()
    {
        return m_nodes;
    }
    
    const std::vector<Node> & GetNodes() const
    {
        return m_nodes;
    }

    num_type TotalHF() const
    {
        num_type total{0};
        for (const auto & node : m_nodes)
            total += node.hf;
        return total;
    }

private:
    std::vector<Node> m_nodes;
};

using namespace generic::ckt;

template <typename num_type>
inline DenseVector<num_type> makeRhs(const ThermalNetwork<num_type> & network, num_type refT)
{
    const size_t nodes = network.Size();
    const size_t source = network.Source();
    DenseVector<num_type> rhs(source);
    for(size_t i = 0, s = 0; i < nodes; ++i) {
        const auto & node = network[i];
        if (node.hf != 0 || node.htc != 0)
            rhs[s++] = node.hf + node.htc * refT;
    }
    return rhs;
}

template <typename num_type>
inline DenseVector<num_type> makeFullRhs(const ThermalNetwork<num_type> & network, num_type refT)
{
    const size_t nodes = network.Size();
    DenseVector<num_type> rhs(nodes);
    for(size_t i = 0 ; i < nodes; ++i) {
        const auto & node = network[i];
        rhs[i] = node.hf + node.htc * refT;
    }
    return rhs;
}

template <typename num_type>
inline SparseMatrix<num_type> makeBondsRhs(const ThermalNetwork<num_type> & network, num_type refT)
{
    using Matrix = SparseMatrix<num_type>;
    using Triplets = std::vector<Eigen::Triplet<num_type> >;

    Triplets triplets;
    const size_t nodes = network.Size();
    for(size_t i = 0 ; i < nodes; ++i) {
        const auto & node = network[i];
        if (node.htc != 0)
            triplets.emplace_back(i, 0, node.htc * refT);
    }
    Matrix rhs(nodes, 1);
    rhs.setFromTriplets(triplets.begin(), triplets.end());
    return rhs;
}

template <typename num_type>
inline SparseMatrix<num_type> makeSourceProjMatrix(const ThermalNetwork<num_type> & network, std::unordered_map<size_t, size_t> & rhs2Nodes)
{
    using Matrix = SparseMatrix<num_type>;
    using Triplets = std::vector<Eigen::Triplet<num_type> >;
    
    rhs2Nodes.clear();
    Triplets triplets;
    const size_t nodes = network.Size();    
    for (size_t i = 0, s = 0; i < nodes; ++i) {
        const auto & node = network[i];
        if (node.hf != 0) {
            rhs2Nodes.emplace(s, i);
            triplets.emplace_back(i, s++, 1);
        }
    }
    auto B = Matrix(nodes, triplets.size());
    B.setFromTriplets(triplets.begin(), triplets.end());
    return B;
}

template <typename num_type>
inline std::pair<SparseMatrix<num_type>, SparseMatrix<num_type>> makeInvCandNegG(const ThermalNetwork<num_type> & network)
{
    using Matrix = SparseMatrix<num_type>;
    using Triplets = std::vector<Eigen::Triplet<num_type> >;
    
    const size_t nodes = network.Size();
    auto invC = Matrix(nodes, nodes);
    auto negG = Matrix(nodes, nodes);
    Triplets tG, tC;
    for (size_t i = 0; i < nodes; ++i) {
        const auto & node = network[i];
        for (size_t j = 0; j < node.ns.size(); ++j) {
            if (auto n = node.ns.at(j); n > i) { //todo, remove ">"" check after modify to single edage 
                if (auto r = node.rs.at(j); r > 0)
                    mna::Stamp(tG, i, n, -num_type{1} / r);
            }
        }
        if (node.htc != 0) mna::Stamp(tG, i, -node.htc);
        if (node.c > 0) mna::Stamp(tC, i, 1 / node.c);
    }
    negG.setFromTriplets(tG.begin(), tG.end());
    invC.setFromTriplets(tC.begin(), tC.end());
    return {invC, negG};
}

template <typename num_type>
inline MNA<SparseMatrix<num_type> > makeMNA(const ThermalNetwork<num_type> & network, const std::set<size_t> & probs = {})
{
    using Matrix = SparseMatrix<num_type>;
    using Triplets = std::vector<Eigen::Triplet<num_type> >;
    
    MNA<Matrix> m;
    const size_t nodes = network.Size();
    const size_t source = network.Source();
    Triplets tG, tC, tB;
    m.G = Matrix(nodes, nodes);
    m.C = Matrix(nodes, nodes);
    m.B = Matrix(nodes, source);
    for (size_t i = 0, s = 0; i < nodes; ++i) {
        const auto & node = network[i];
        for (size_t j = 0; j < node.ns.size(); ++j) {
            if (auto n = node.ns.at(j); n > i) { //todo, remove ">"" check after modify to single edage 
                if (auto r = node.rs.at(j); r > 0)
                    mna::Stamp(tG, i, n, num_type{1} / r);
            }
        }
        if (node.htc != 0) mna::Stamp(tG, i, node.htc);
        if (node.c > 0) mna::Stamp(tC, i, node.c);
        if (node.hf != 0 || node.htc != 0)
            tB.emplace_back(i, s++, 1);
    }
    m.G.setFromTriplets(tG.begin(), tG.end());
    m.C.setFromTriplets(tC.begin(), tC.end());
    m.B.setFromTriplets(tB.begin(), tB.end());

    if (probs.empty()) {
        m.L = Matrix(nodes, nodes);
        m.L.setIdentity();
    }
    else {
        size_t j{0};
        Triplets tL;
        for (auto p : probs) {
            GENERIC_ASSERT(p < nodes)
            tL.emplace_back(p, j++, 1);
        }
        m.L = Matrix(nodes, probs.size());
        m.L.setFromTriplets(tL.begin(), tL.end());
    }
    return m;
}

}//namespace model
}//namespace thermal