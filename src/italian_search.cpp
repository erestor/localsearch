//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/italian_search.h>
#include <algorithm/factory.h>
#include <algorithm/isolution.h>
#include <ctoolhu/event/firer.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace Ctoolhu;
using namespace boost::property_tree;
using namespace std;

namespace Algorithm { namespace ItalianSearch {

namespace {
	//register with algorithm factory
	const bool registered = SingleFactory::Instance().Register("italian_search",
		[](const ptree &pt) {
			return make_unique<ItalianSearch>(pt);
		});
}

ItalianSearch::ItalianSearch(const ptree &configPt)
:
	_configPt(configPt)
{
	_config.cycles = _configPt.get("italian.cycles", 4);
	_config.generationName		= _configPt.get("italian.algorithm.generation", "generation");
	_config.roomRNAName			= _configPt.get("italian.algorithm.alg1", "room_rna");
	_config.roomTabuSearchName	= _configPt.get("italian.algorithm.alg2", "room_tabu");
	_config.timeRNAName			= _configPt.get("italian.algorithm.alg3", "time_rna");
	_config.timeTabuSearchName	= _configPt.get("italian.algorithm.alg4", "time_tabu");
}

const string &ItalianSearch::Name() const
{
	static string name { "Italian search" };
	return name;
}

bool ItalianSearch::Run(solution_ptr_type solutionPtr)
{
	auto initialFitness = solutionPtr->GetFitness();
	{
		//prelude is the generation algorithm
		auto generationSearchPtr = SingleFactory::Instance().CreateAlgorithm(_config.generationName, _configPt.get_child("generation"));
		generationSearchPtr->SetParent(this);
		generationSearchPtr->Start(solutionPtr);
	}

	auto roomRNASearchPtr = SingleFactory::Instance().CreateAlgorithm(_config.roomRNAName, _configPt.get_child("room_rna"));
	auto roomTabuSearchPtr = SingleFactory::Instance().CreateAlgorithm(_config.roomTabuSearchName, _configPt.get_child("room_tabu"));
	auto timeRNASearchPtr = SingleFactory::Instance().CreateAlgorithm(_config.timeRNAName, _configPt.get_child("time_rna"));
	auto timeTabuSearchPtr = SingleFactory::Instance().CreateAlgorithm(_config.timeTabuSearchName, _configPt.get_child("time_tabu"));
	roomRNASearchPtr->SetParent(this);
	roomTabuSearchPtr->SetParent(this);
	timeRNASearchPtr->SetParent(this);
	timeTabuSearchPtr->SetParent(this);

	int IdleCycles = 0;
	while (!IsStopRequested() && (int)solutionPtr->GetFitness() > 0 && (_config.cycles < 0 || IdleCycles < _config.cycles)) {
		IdleCycles++;

		if (roomRNASearchPtr->Start(solutionPtr))
			IdleCycles = 0;

		if (roomTabuSearchPtr->Start(solutionPtr))
			IdleCycles = 0;

		if (timeRNASearchPtr->Start(solutionPtr))
			IdleCycles = 0;

		if (timeTabuSearchPtr->Start(solutionPtr))
			IdleCycles = 0;
	}	
	return solutionPtr->GetFitness() < initialFitness;
}

} } //ns Algorithm::ItalianSearch
