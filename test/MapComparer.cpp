/*
 * MapComparer.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include <boost/test/unit_test.hpp>

#include "MapComparer.h"

#include "../lib/ScopeGuard.h"
#include "../lib/mapping/CMap.h"

#define VCMI_CHECK_FIELD_EQUAL_P(field) BOOST_CHECK_EQUAL(actual->field, expected->field)

#define VCMI_CHECK_FIELD_EQUAL(field) BOOST_CHECK_EQUAL(actual.field, expected.field)

#define VCMI_REQUIRE_FIELD_EQUAL_P(field) BOOST_REQUIRE_EQUAL(actual->field, expected->field)
#define VCMI_REQUIRE_FIELD_EQUAL(field) BOOST_REQUIRE_EQUAL(actual.field, expected.field)

template <class T>
void checkEqual(const T & actual, const T & expected)
{
	BOOST_CHECK_EQUAL(actual, expected)	;
}

void checkEqual(const std::vector<bool> & actual, const std::vector<bool> & expected)
{
	BOOST_CHECK_EQUAL(actual.size(), expected.size());

	for(auto actualIt = actual.begin(), expectedIt = expected.begin(); actualIt != actual.end() && expectedIt != expected.end(); actualIt++, expectedIt++)
	{
		checkEqual(*actualIt, *expectedIt);
	}
}

template <class Element>
void checkEqual(const std::vector<Element> & actual, const std::vector<Element> & expected)
{
	BOOST_CHECK_EQUAL(actual.size(), expected.size());

	for(auto actualIt = actual.begin(), expectedIt = expected.begin(); actualIt != actual.end() && expectedIt != expected.end(); actualIt++, expectedIt++)
	{
		checkEqual(*actualIt, *expectedIt);
	}
}

template <class Element>
void checkEqual(const std::set<Element> & actual, const std::set<Element> & expected)
{
	BOOST_CHECK_EQUAL(actual.size(), expected.size());

	for(auto elem : expected)
	{
		if(!vstd::contains(actual, elem))
			BOOST_ERROR("Required element not found "+boost::to_string(elem));
	}
}

void checkEqual(const SHeroName & actual, const SHeroName & expected)
{
	VCMI_CHECK_FIELD_EQUAL(heroId);
	VCMI_CHECK_FIELD_EQUAL(heroName);
}

void checkEqual(const PlayerInfo & actual, const PlayerInfo & expected)
{
	VCMI_CHECK_FIELD_EQUAL(canHumanPlay);
	VCMI_CHECK_FIELD_EQUAL(canComputerPlay);
	VCMI_CHECK_FIELD_EQUAL(aiTactic);

	checkEqual(actual.allowedFactions, expected.allowedFactions);

	VCMI_CHECK_FIELD_EQUAL(isFactionRandom);
	VCMI_CHECK_FIELD_EQUAL(mainCustomHeroPortrait);
	VCMI_CHECK_FIELD_EQUAL(mainCustomHeroName);

	VCMI_CHECK_FIELD_EQUAL(mainCustomHeroId);

	checkEqual(actual.heroesNames, expected.heroesNames);

	VCMI_CHECK_FIELD_EQUAL(hasMainTown);
	VCMI_CHECK_FIELD_EQUAL(generateHeroAtMainTown);
	VCMI_CHECK_FIELD_EQUAL(posOfMainTown);
	VCMI_CHECK_FIELD_EQUAL(team);
	VCMI_CHECK_FIELD_EQUAL(hasRandomHero);
}

void checkEqual(const EventExpression & actual,  const EventExpression & expected)
{
	//todo: checkEventExpression
}

void checkEqual(const TriggeredEvent & actual,  const TriggeredEvent & expected)
{
	VCMI_CHECK_FIELD_EQUAL(identifier);
	VCMI_CHECK_FIELD_EQUAL(description);
	VCMI_CHECK_FIELD_EQUAL(onFulfill);
	VCMI_CHECK_FIELD_EQUAL(effect.type);
	VCMI_CHECK_FIELD_EQUAL(effect.toOtherMessage);

	checkEqual(actual.trigger, expected.trigger);
}

void checkEqual(const Rumor & actual, const Rumor & expected)
{
	VCMI_CHECK_FIELD_EQUAL(name);
	VCMI_CHECK_FIELD_EQUAL(text);
}

void checkEqual(const DisposedHero & actual, const DisposedHero & expected)
{
	VCMI_CHECK_FIELD_EQUAL(heroId);
	VCMI_CHECK_FIELD_EQUAL(portrait);
	VCMI_CHECK_FIELD_EQUAL(name);
	VCMI_CHECK_FIELD_EQUAL(players);

}

void checkEqual(const ObjectTemplate & actual, const ObjectTemplate & expected)
{
	VCMI_CHECK_FIELD_EQUAL(id);
	VCMI_CHECK_FIELD_EQUAL(subid);
	VCMI_CHECK_FIELD_EQUAL(printPriority);
	VCMI_CHECK_FIELD_EQUAL(animationFile);
	//VCMI_CHECK_FIELD_EQUAL(stringID);
}

void checkEqual(const TerrainTile & actual, const TerrainTile & expected)
{
	//fatal fail here on any error
	VCMI_REQUIRE_FIELD_EQUAL(terType);
	VCMI_REQUIRE_FIELD_EQUAL(terView);
	VCMI_REQUIRE_FIELD_EQUAL(riverType);
	VCMI_REQUIRE_FIELD_EQUAL(riverDir);
	VCMI_REQUIRE_FIELD_EQUAL(roadType);
	VCMI_REQUIRE_FIELD_EQUAL(roadDir);
	VCMI_REQUIRE_FIELD_EQUAL(extTileFlags);

	BOOST_REQUIRE_EQUAL(actual.blockingObjects.size(), expected.blockingObjects.size());
	BOOST_REQUIRE_EQUAL(actual.visitableObjects.size(), expected.visitableObjects.size());

	VCMI_REQUIRE_FIELD_EQUAL(visitable);
	VCMI_REQUIRE_FIELD_EQUAL(blocked);

}

//MapComparer
void MapComparer::compareHeader()
{
	//map size parameters are vital for further checks
	VCMI_REQUIRE_FIELD_EQUAL_P(height);
	VCMI_REQUIRE_FIELD_EQUAL_P(width);
	VCMI_REQUIRE_FIELD_EQUAL_P(twoLevel);

	VCMI_CHECK_FIELD_EQUAL_P(name);
	VCMI_CHECK_FIELD_EQUAL_P(description);
	VCMI_CHECK_FIELD_EQUAL_P(difficulty);
	VCMI_CHECK_FIELD_EQUAL_P(levelLimit);

	VCMI_CHECK_FIELD_EQUAL_P(victoryMessage);
	VCMI_CHECK_FIELD_EQUAL_P(defeatMessage);
	VCMI_CHECK_FIELD_EQUAL_P(victoryIconIndex);
	VCMI_CHECK_FIELD_EQUAL_P(defeatIconIndex);

	VCMI_CHECK_FIELD_EQUAL_P(howManyTeams);

	checkEqual(actual->players, expected->players);

	checkEqual(actual->allowedHeroes, expected->allowedHeroes);

	std::vector<TriggeredEvent> actualEvents = actual->triggeredEvents;
	std::vector<TriggeredEvent> expectedEvents = expected->triggeredEvents;

	auto sortByIdentifier = [](const TriggeredEvent & lhs, const TriggeredEvent & rhs) -> bool
	{
		return lhs.identifier  < rhs.identifier;
	};
	boost::sort (actualEvents, sortByIdentifier);
	boost::sort (expectedEvents, sortByIdentifier);

	checkEqual(actualEvents, expectedEvents);
}

void MapComparer::compareOptions()
{
	checkEqual(actual->rumors, expected->rumors);
	checkEqual(actual->disposedHeroes, expected->disposedHeroes);
	//todo: compareOptions predefinedHeroes

	checkEqual(actual->allowedAbilities, expected->allowedAbilities);
	checkEqual(actual->allowedArtifact, expected->allowedArtifact);
	checkEqual(actual->allowedSpell, expected->allowedSpell);

	//todo: compareOptions  events
}

void MapComparer::compareObject(const CGObjectInstance * actual, const CGObjectInstance * expected)
{
	BOOST_CHECK_EQUAL(actual->instanceName, expected->instanceName);
	BOOST_CHECK_EQUAL(typeid(actual).name(), typeid(expected).name());//todo: remove and use just comparison

	std::string actualFullID = boost::to_string(boost::format("%s(%d)|%s(%d) %d") % actual->typeName % actual->ID % actual->subTypeName % actual->subID % actual->tempOwner);
	std::string expectedFullID = boost::to_string(boost::format("%s(%d)|%s(%d) %d") % expected->typeName % expected->ID % expected->subTypeName % expected->subID % expected->tempOwner);

	BOOST_CHECK_EQUAL(actualFullID, expectedFullID);

	VCMI_CHECK_FIELD_EQUAL_P(pos);
	checkEqual(actual->appearance, expected->appearance);
}

void MapComparer::compareObjects()
{
	BOOST_CHECK_EQUAL(actual->objects.size(), expected->objects.size());

	for(size_t idx = 0; idx < expected->objects.size(); idx++)
	{
		auto expectedObject = expected->objects[idx];

		BOOST_REQUIRE_EQUAL(idx, expectedObject->id.getNum());

		{
			auto it = expected->instanceNames.find(expectedObject->instanceName);

			BOOST_REQUIRE(it != expected->instanceNames.end());
		}

		{
			auto it = actual->instanceNames.find(expectedObject->instanceName);

			BOOST_REQUIRE(it != actual->instanceNames.end());

			auto actualObject = it->second;

			compareObject(actualObject, expectedObject);
		}
	}
}

void MapComparer::compareTerrain()
{
	//assume map dimensions check passed
	//todo: separate check for underground

	for(int x = 0; x < expected->width; x++)
		for(int y = 0; y < expected->height; y++)
		{
			int3 coord(x,y,0);
			BOOST_TEST_CHECKPOINT(coord);

			checkEqual(actual->getTile(coord), expected->getTile(coord));
		}
}

void MapComparer::compare()
{
	BOOST_REQUIRE_NE((void *) actual, (void *) expected); //should not point to the same object
	BOOST_REQUIRE_MESSAGE(actual != nullptr, "Actual map is not defined");
	BOOST_REQUIRE_MESSAGE(expected != nullptr, "Expected map is not defined");

	compareHeader();
	compareOptions();
	compareObjects();
	compareTerrain();
}

void MapComparer::operator() (const std::unique_ptr<CMap>& actual, const std::unique_ptr<CMap>&  expected)
{
	this->actual = actual.get();
	this->expected = expected.get();
	compare();
}

//JsonMapComparer
JsonMapComparer::JsonMapComparer():
	strict(false)
{

}

vstd::ScopeGuard<JsonMapComparer::TScopeGuard> JsonMapComparer::pushName(const std::string & name)
{
	namePath.push_back(name);
	return vstd::makeScopeGuard<TScopeGuard>([this](){namePath.pop_back();});
}

std::string JsonMapComparer::buildMessage(const std::string & message)
{
	std::stringstream buf;

	for(auto & s : namePath)
		buf << s << "|";
	buf << " " << message;
	return buf.str();
}

void JsonMapComparer::addError(const std::string & message)
{
	BOOST_ERROR(buildMessage(message));
}

void JsonMapComparer::addWarning(const std::string & message)
{
	BOOST_WARN_MESSAGE(false, buildMessage(message));
}

bool JsonMapComparer::isEmpty(const JsonNode & value)
{
	switch (value.getType())
	{
	case JsonNode::DATA_NULL:
		return true;
	case JsonNode::DATA_BOOL:
		return !value.Bool();
	case JsonNode::DATA_FLOAT:
		return value.Float() == 0;
	case JsonNode::DATA_STRING:
		return value.String() == "";
	case JsonNode::DATA_VECTOR:
		return value.Vector().empty();
	case JsonNode::DATA_STRUCT:
		return value.Struct().empty();
		break;
	default:
		BOOST_FAIL("Unknown Json type");
		return false;
	}
}

void JsonMapComparer::check(const bool condition, const std::string & message)
{
	if(strict)
		BOOST_REQUIRE_MESSAGE(condition, buildMessage(message));
	else
		BOOST_CHECK_MESSAGE(condition, buildMessage(message));
}

void JsonMapComparer::checkEqualInteger(const si64 actual, const si64 expected)
{
	if(actual != expected)
	{
		check(false, boost::str(boost::format("'%d' != '%d'") % actual % expected));
	}
}

void JsonMapComparer::checkEqualFloat(const double actual, const double expected)
{
	if(std::abs(actual - expected) > 1e-6)
	{
		check(false, boost::str(boost::format("'%d' != '%d' (diff %d)") % actual % expected % (expected - actual)));
	}
}

void JsonMapComparer::checkEqualString(const std::string & actual, const std::string & expected)
{
	if(actual != expected)
	{
		check(false, boost::str(boost::format("'%s' != '%s'") % actual % expected));
	}
}

void JsonMapComparer::checkEqualJson(const JsonMap & actual, const JsonMap & expected)
{
    for(const auto & p : expected)
		checkStructField(actual, p.first, p.second);
	for(const auto & p : actual)
		checkExcessStructField(p.second, p.first, expected);
}

void JsonMapComparer::checkEqualJson(const JsonVector & actual, const JsonVector & expected)
{
	check(actual.size() == expected.size(), "size mismatch");

	size_t sz = std::min(actual.size(), expected.size());

	for(size_t idx = 0; idx < sz; idx ++)
	{
		auto guard = pushName(boost::to_string(idx));

		checkEqualJson(actual.at(idx), expected.at(idx));
	}
}

void JsonMapComparer::checkEqualJson(const JsonNode & actual, const JsonNode & expected)
{
	//name has been pushed before

	const bool validType = actual.getType() == expected.getType();

	if(!validType)
		addError("type mismatch");

	//do detail checks avoiding assertions in JsonNode
	if(validType)
	{
		switch (actual.getType())
		{
		case JsonNode::DATA_NULL:
			break; //do nothing
		case JsonNode::DATA_BOOL:
			check(actual.Bool() == expected.Bool(), "mismatch");
			break;
		case JsonNode::DATA_FLOAT:
			checkEqualFloat(actual.Float(),expected.Float());
			break;
		case JsonNode::DATA_STRING:
			checkEqualString(actual.String(),expected.String());
			break;
		case JsonNode::DATA_VECTOR:
			checkEqualJson(actual.Vector(), expected.Vector());
			break;
		case JsonNode::DATA_STRUCT:
			checkEqualJson(actual.Struct(), expected.Struct());
			break;
		case JsonNode::DATA_INTEGER:
			checkEqualInteger(actual.Integer(), expected.Integer());
			break;
		default:
			BOOST_FAIL("Unknown Json type");
			break;
		}
	}
}

void JsonMapComparer::checkExcessStructField(const JsonNode & actualValue, const std::string & name, const JsonMap & expected)
{
	auto guard = pushName(name);

	if(!vstd::contains(expected, name))
	{
		if(isEmpty(actualValue))
			addWarning("excess");
		else
			addError("excess");
	}
}

void JsonMapComparer::checkStructField(const JsonMap & actual, const std::string & name, const JsonNode & expectedValue)
{
	auto guard = pushName(name);

	if(!vstd::contains(actual, name))
	{
		if(isEmpty(expectedValue))
			addWarning("missing");
		else
			addError("missing");
	}
	else
		checkEqualJson(actual.at(name), expectedValue);
}

void JsonMapComparer::compareHeader(const JsonNode & actual, const JsonNode & expected)
{
	strict = false;
	auto guard = pushName("hdr");
	checkEqualJson(actual, expected);
}

void JsonMapComparer::compareObjects(const JsonNode & actual, const JsonNode & expected)
{
	strict = false;
	auto guard = pushName("obj");
	checkEqualJson(actual, expected);
}

void JsonMapComparer::compareTerrain(const std::string & levelName, const JsonNode & actual, const JsonNode & expected)
{
	strict = true;
	auto guard = pushName(levelName);
	checkEqualJson(actual, expected);
}
