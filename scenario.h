#pragma once
#include <vector>
struct Scenario
{
	char* name;
	bool hasEnterAnim;
};

std::vector<Scenario> getAllGTAScenarios();