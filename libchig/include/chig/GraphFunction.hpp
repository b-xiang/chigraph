#ifndef CHIG_GRAPH_FUNCTION_HPP
#define CHIG_GRAPH_FUNCTION_HPP

#pragma once

#include "chig/Fwd.hpp"
#include "chig/Graph.hpp"
#include "chig/ToString.hpp"
#include "chig/json.hpp"

#include <llvm/IR/Module.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/optional.hpp>

namespace chig
{
/// this is an AST-like representation of a function in a graph
/// It is used for IDE-like behavior, codegen, and JSON generation.
struct GraphFunction {
	/// Construct the graph
	/// Also constructs a input node
	/// \param mod The owning module
	/// \param name The name of the function
	GraphFunction(JsonModule& mod, std::string name,
		std::vector<std::pair<DataType, std::string>> ins,
		std::vector<std::pair<DataType, std::string>> outs);

	/// Destructor
	~GraphFunction();

	/// Constructs a GraphFunction from a JOSN object
	/// \param data The JSON object to read from
	/// \param module The module to create the GraphFunction with
	/// \param ret_func The GraphFunction that has been produced
	/// \return The result
	static Result fromJSON(
		JsonModule& module, const nlohmann::json& data, std::unique_ptr<GraphFunction>* ret_func);

	/// Serialize the GraphFunction to JSON (usually called from JsonModule::toJson)
	/// \param toFill The JSON object representing the graph
	/// \return The result
	Result toJSON(nlohmann::json* toFill) const;

	/// Compile the graph to an \c llvm::Function (usually called from JsonModule::generateModule)
	/// \param mod The module to codgen into, should already be a valid module
	/// \param ret_func The \c llvm::Function that it was compiled to
	/// \ret The result
	Result compile(llvm::Module* mod, llvm::Function** ret_func) const;

	/// Gets the node with type lang:entry
	/// returns nullptr on failure
	/// Also returns nullptr if there are two entry nodes, which is illegal
	/// \return the entry node
	NodeInstance* entryNode() const noexcept;

	/// Add a node to the graph
	/// \param type The type of the node
	/// \param x The x location of the node
	/// \param y The y location of the node
	/// \param id The node ID
	NodeInstance* insertNode(
		std::unique_ptr<NodeType> type, float x, float y, const std::string& id)
	{
		return graph().insertNode(std::move(type), x, y, id);
	}

	/// Get the LLVM function type for the function
	/// \return The function type
	llvm::FunctionType* functionType() const;

	/// Load the graph from the source json
	/// \return The result
	Result loadGraph();

	/// Get the context
	/// \return The context
	const Context& context() const { return *mContext; }
	/// \copydoc chig::GraphFunction::context() const
	Context& context() { return *mContext; }
	/// Get the name of the function
	/// \return The name of the function
	std::string name() const { return mName; }
	/// Get the function inputs in the format {type, docstring}
	/// \return The inputs
	const std::vector<std::pair<DataType, std::string>>& inputs() const { return mInputs; }
	/// Get the function outputs in the format {type, docstring}
	/// \return The outputs
	const std::vector<std::pair<DataType, std::string>>& outputs() const { return mOutputs; }
	/// Get the graph
	/// \return The graph
	const Graph& graph() const { return mGraph; }
	/// \copydoc chig::GraphFunction::graph() const
	Graph& graph() { return mGraph; }
	const JsonModule& module() const { return *mModule; }
	JsonModule& module() { return *mModule; }
private:
	JsonModule* mModule;
	Context* mContext;
	std::string mName;  /// the name of the function

	std::vector<std::pair<DataType, std::string>> mInputs;
	std::vector<std::pair<DataType, std::string>> mOutputs;

	nlohmann::json mSource;
	Graph mGraph;
};

inline std::pair<std::string, std::string> parseColonPair(const std::string& in)
{
	size_t colonID = in.find(':');
	if (colonID == std::string::npos) {
		return {};
	}
	std::string module = in.substr(0, in.find(':'));
	std::string name = in.substr(in.find(':') + 1);

	return {module, name};
}
}

#endif  // CHIG_GRAPH_FUNCTION_HPP
