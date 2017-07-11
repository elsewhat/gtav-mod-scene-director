#include "StageLight.h"
#include "tinyxml2.h"
#include <vector>
#include <algorithm>


std::vector<GTAObject> sceneDirectorLightObject = {
	getGTAObjectFromObjName("prop_sd_spot_01"),
	getGTAObjectFromObjName("prop_sd_spot_02"),
	getGTAObjectFromObjName("prop_sd_spot_03"),
	getGTAObjectFromObjName("prop_sd_spot_04"),
	getGTAObjectFromObjName("prop_sd_spot_05"),
	getGTAObjectFromObjName("prop_sd_spot_06"),
	getGTAObjectFromObjName("prop_sd_spot_07"),
	getGTAObjectFromObjName("prop_sd_spot_08"),
	getGTAObjectFromObjName("prop_sd_spot_09"),
	getGTAObjectFromObjName("prop_sd_spot_10"),
	getGTAObjectFromObjName("prop_sd_spot_11"),
	getGTAObjectFromObjName("prop_sd_spot_12"),
	getGTAObjectFromObjName("prop_sd_spot_13"),
	getGTAObjectFromObjName("prop_sd_spot_14"),
	getGTAObjectFromObjName("prop_sd_spot_15"),
};

std::string stageLightConfigFileName = "SceneDirectorStageLights_config.xml";

std::vector<StageLightRotationType> stageLightRotationTypes = { { "None", true } };
std::vector<StageLightMovementType> stageLightMovementTypes = { { "None", true } };
std::vector<StageLightFlickerType> stageLightFlickerTypes = { { "None", true } };

std::vector<GTAObject> getSceneDirectorLightObject() {
	return sceneDirectorLightObject;
}

GTAObject getDefaultSceneDirectorLightObject() {
	return getGTAObjectFromObjName("prop_sd_spot_01");
}

GTAObject getNextSceneDirectorLightObject(GTAObject lightObject) {

	//see http://stackoverflow.com/questions/14225932/search-for-a-struct-item-in-a-vector-by-member-data
	int foundIndex = std::find_if(sceneDirectorLightObject.begin(), sceneDirectorLightObject.end(), [=](GTAObject const& aLightObject) {
		return (aLightObject.title.compare(lightObject.title) ==0) ;
	}) - sceneDirectorLightObject.begin();
	if (foundIndex + 1 >= sceneDirectorLightObject.size()) {//aLightObject not found or in last element
		return sceneDirectorLightObject[0];
	}
	else {
		return sceneDirectorLightObject[foundIndex + 1];
	}
}

std::vector<StageLightRotationType> getStageLightRotationTypes()
{
	return stageLightRotationTypes;
}

StageLightRotationType getDefaultStageLightRotationType()
{
	return stageLightRotationTypes[0];
}

StageLightRotationType getNextStageLightRotationType(StageLightRotationType stageLightEvent)
{
	//see http://stackoverflow.com/questions/14225932/search-for-a-struct-item-in-a-vector-by-member-data
	int foundIndex = std::find_if(stageLightRotationTypes.begin(), stageLightRotationTypes.end(), [=](StageLightRotationType const& aStageLightEvent) {
		return (aStageLightEvent.name.compare(stageLightEvent.name) == 0);
	}) - stageLightRotationTypes.begin();
	if (foundIndex + 1 >= stageLightRotationTypes.size()) {//aLightObject not found or in last element
		return stageLightRotationTypes[0];
	}
	else {
		return stageLightRotationTypes[foundIndex + 1];
	}
}

std::vector<StageLightMovementType> getStageLightMovementTypes()
{
	return stageLightMovementTypes;
}

StageLightMovementType getDefaultStageLightMovementType()
{
	return stageLightMovementTypes[0];
}

StageLightMovementType getNextStageLightMovementType(StageLightMovementType stageLightEvent)
{
	//see http://stackoverflow.com/questions/14225932/search-for-a-struct-item-in-a-vector-by-member-data
	int foundIndex = std::find_if(stageLightMovementTypes.begin(), stageLightMovementTypes.end(), [=](StageLightMovementType const& aStageLightEvent) {
		return (aStageLightEvent.name.compare(stageLightEvent.name) == 0);
	}) - stageLightMovementTypes.begin();
	if (foundIndex + 1 >= stageLightMovementTypes.size()) {//aLightObject not found or in last element
		return stageLightMovementTypes[0];
	}
	else {
		return stageLightMovementTypes[foundIndex + 1];
	}
}

std::vector<StageLightFlickerType> getStageLightFlickerTypes()
{
	return stageLightFlickerTypes;
}

StageLightFlickerType getDefaultStageLightFlickerType()
{
	return stageLightFlickerTypes[0];
}

StageLightFlickerType getNextStageLightFlickerType(StageLightFlickerType stageLightEvent)
{
	
	//see http://stackoverflow.com/questions/14225932/search-for-a-struct-item-in-a-vector-by-member-data
	int foundIndex = std::find_if(stageLightFlickerTypes.begin(), stageLightFlickerTypes.end(), [=](StageLightFlickerType const& aStageLightEvent) {
		return (aStageLightEvent.name.compare(stageLightEvent.name) == 0);
	}) - stageLightFlickerTypes.begin();

	log_to_file("NextFlicker size " + std::to_string(stageLightFlickerTypes.size()) + " found index : "+std::to_string(foundIndex));
	if (foundIndex + 1 >= stageLightFlickerTypes.size()) {
		return stageLightFlickerTypes[0];
	}
	else {
		return stageLightFlickerTypes[foundIndex + 1];
	}
}

void initializeStageLights()
{
		log_to_file("initializeStageLights");

		tinyxml2::XMLDocument doc = new tinyxml2::XMLDocument();
		doc.LoadFile(stageLightConfigFileName.c_str());
		if (doc.Error()) {
			set_status_text("Save file " + stageLightConfigFileName + " could not be loaded. Error: " + doc.ErrorName());
			return;
		}

		stageLightFlickerTypes.clear();
		stageLightRotationTypes.clear();
		stageLightMovementTypes.clear();

		//Start with None null object
		stageLightFlickerTypes.push_back({ "None", true });
		stageLightRotationTypes.push_back({ "None",true });
		stageLightMovementTypes.push_back({ "None",true });

		tinyxml2::XMLElement* sceneDirectorElement = doc.RootElement();

		for (tinyxml2::XMLElement* flickerTypeElement = sceneDirectorElement->FirstChildElement("FlickerType");
		flickerTypeElement;
			flickerTypeElement = flickerTypeElement->NextSiblingElement("FlickerType"))
		{
			StageLightFlickerType flickerType;
			flickerType.name =  flickerTypeElement->Attribute("name");
			flickerType.isNull = false;
			log_to_file("FlickerType " + flickerType.name);

			std::vector<StageLightFlicker> lightEvents;

			for (tinyxml2::XMLElement* flickerElement = flickerTypeElement->FirstChildElement("Flicker");
			flickerElement;
				flickerElement = flickerElement->NextSiblingElement("Flicker"))
			{

				StageLightFlicker flickerEvent;
				flickerEvent.isOn = true;
				int length = 10000;
				tinyxml2::XMLError result = flickerElement->QueryIntAttribute("on", &length);
				if (result == tinyxml2::XML_NO_ATTRIBUTE) {
					tinyxml2::XMLError result = flickerElement->QueryIntAttribute("off", &length);
					flickerEvent.isOn = false;
				}
				flickerEvent.length = length;
				lightEvents.push_back(flickerEvent);
			}

			flickerType.flickerEvents = lightEvents;
			stageLightFlickerTypes.push_back(flickerType);
		}

		for (tinyxml2::XMLElement* rotationTypeElement = sceneDirectorElement->FirstChildElement("RotationType");
		rotationTypeElement;
			rotationTypeElement = rotationTypeElement->NextSiblingElement("RotationType"))
		{
			StageLightRotationType rotationType;
			rotationType.name = rotationTypeElement->Attribute("name");
			rotationType.isNull = false;
			std::vector<StageLightRotation> lightEvents;

			for (tinyxml2::XMLElement* rotationElement = rotationTypeElement->FirstChildElement("Rotation");
			rotationElement;
				rotationElement = rotationElement->NextSiblingElement("Rotation"))
			{
				StageLightRotation rotationEvent;
				rotationEvent.hasPitch = true;
				rotationEvent.hasYaw = true;

				float pitch = 0;
				tinyxml2::XMLError result = rotationElement->QueryFloatAttribute("pitch", &pitch);
				if (result == tinyxml2::XML_NO_ATTRIBUTE) {
					rotationEvent.hasPitch = false;
				}
				else {
					rotationEvent.pitch = pitch;
				}

				float yaw = 0;
				result = rotationElement->QueryFloatAttribute("yaw", &yaw);
				if (result == tinyxml2::XML_NO_ATTRIBUTE) {
					rotationEvent.hasYaw = false;
				}
				else {
					rotationEvent.yaw = yaw;
				}

				int length = rotationElement->IntAttribute("length");
				rotationEvent.length = length;

				lightEvents.push_back(rotationEvent);
			}
			rotationType.rotationEvents = lightEvents;

			stageLightRotationTypes.push_back(rotationType);
		}

		for (tinyxml2::XMLElement* movementTypeElement = sceneDirectorElement->FirstChildElement("MovementType");
		movementTypeElement;
			movementTypeElement = movementTypeElement->NextSiblingElement("MovementType"))
		{

			StageLightMovementType movementType;
			movementType.name = movementTypeElement->Attribute("name");
			movementType.isNull = false;
			std::vector<StageLightMovement> lightEvents;

			for (tinyxml2::XMLElement* movementElement = movementTypeElement->FirstChildElement("Movement");
			movementElement;
				movementElement = movementElement->NextSiblingElement("Movement"))
			{

				StageLightMovement movementEvent;
				Vector3 movementDelta;
				float x = 0.0, y = 0.0, z = 0.0;
				movementElement->QueryFloatAttribute("x", &x);
				movementElement->QueryFloatAttribute("y", &y);
				movementElement->QueryFloatAttribute("z", &z);
				movementDelta.x = x;
				movementDelta.y = y;
				movementDelta.z = z;

				int length = movementElement->IntAttribute("length");
				movementEvent.length = length;
				movementEvent.movementDelta = movementDelta;

				float xyRatio = 0.0;
				tinyxml2::XMLError result = movementElement->QueryFloatAttribute("xyRatio", &xyRatio);
				if (result == tinyxml2::XML_SUCCESS) {
					movementEvent.hasxyRatio = true;
					movementEvent.xyRatio = xyRatio;
				}
				else {
					movementEvent.hasxyRatio = false;
				}

				float yxRatio = 0.0;
				result = movementElement->QueryFloatAttribute("yxRatio", &xyRatio);
				if (result == tinyxml2::XML_SUCCESS) {
					movementEvent.hasyxRatio = true;
					movementEvent.yxRatio = yxRatio;
				}
				else {
					movementEvent.hasyxRatio = false;
				}
				lightEvents.push_back(movementEvent);
			}

			movementType.movementEvents = lightEvents;

			stageLightMovementTypes.push_back(movementType);
		}
}


void StageLight::moveLight(Vector3 lightPosition)
{
	m_lightPosition = lightPosition;
	//update current position (rotation ignored for now)
	if (m_lightObject.objReference != 0) {
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(m_lightObject.objReference, m_lightPosition.x, m_lightPosition.y, m_lightPosition.z, 0, 0, 1);
	}
}

void StageLight::moveInitialPosOfLight(Vector3 lightPosition)
{
	m_initialLightPosition = lightPosition;
	moveLight(m_initialLightPosition);
}

void StageLight::rotateLight(Vector3 lightRotation)
{
	m_lightRotation = lightRotation;
	//update current position (rotation ignored for now)
	if (m_lightObject.objReference != 0) {
		ENTITY::SET_ENTITY_ROTATION(m_lightObject.objReference, m_lightRotation.x + 180, 0, m_lightRotation.z, 2, true);
		//log_to_file("Rotating light pitch:" + std::to_string(m_lightRotation.x + 180) + " yaw:"+ std::to_string(m_lightRotation.z));
	}
}

void StageLight::rotateInitialRotationOfLight(Vector3 lightRotation)
{
	m_initialLightRotation = lightRotation;
	rotateLight(m_initialLightRotation);
}

void StageLight::swapLightObject(GTAObject newLightObject)
{
	//update current position and rotation
	if (m_lightObject.objReference == 0) {
		Vector3 currentPosition = ENTITY::GET_ENTITY_COORDS(m_lightObject.objReference, true);
		m_lightPosition = currentPosition;
	}
	//delete old object
	if (m_lightObject.objReference != 0) {
		OBJECT::DELETE_OBJECT(&m_lightObject.objReference);
	}
	m_lightObject = newLightObject;

	//create object
	if (m_lightObject.objHash == -1) {
		m_lightObject.objHash = GAMEPLAY::GET_HASH_KEY(m_lightObject.objName);
	}

	STREAMING::REQUEST_MODEL(m_lightObject.objHash);
	DWORD ticksStart = GetTickCount();
	while (!STREAMING::HAS_MODEL_LOADED(m_lightObject.objHash))
	{
		WAIT(0);
		if (GetTickCount() - ticksStart > 5000) {
			log_to_file("Failed to load GTAObject name:" + std::string(m_lightObject.objName) + " Hash:" + std::to_string(m_lightObject.objHash));
			set_status_text("Failed to load objects");
			return;
		}
	}

	int newObjectRef = OBJECT::CREATE_OBJECT(m_lightObject.objHash, m_lightPosition.x, m_lightPosition.y, m_lightPosition.z, true, true, false);
	ENTITY::SET_ENTITY_ROTATION(newObjectRef, m_lightRotation.x + 180, 0, m_lightRotation.z, 2, true);

	m_lightObject.objReference = newObjectRef;
}

void StageLight::resetToInitial()
{
	moveLight(m_initialLightPosition);
	if (m_lightRotation.x != m_initialLightRotation.x || m_lightRotation.z != m_initialLightRotation.z) {
		rotateLight(m_initialLightRotation);
	}
	m_flickerStart = 0;
	m_flickerIndex = 0;

	m_rotationStart = 0;
	m_rotationIndex = 0;

	m_movementStart = 0;
	m_movementIndex = 0;

}

void StageLight::setHasFlicker(std::vector<StageLightFlicker> flickerEvents)
{
	m_hasFlicker = true;
	m_flickerEvents = flickerEvents;
}

std::vector<StageLightFlicker> StageLight::getFlickerEvents()
{
	return m_flickerEvents;
}

void StageLight::stopFlicker()
{
	m_hasFlicker = false;
}

StageLight::StageLight(Vector3 lightPosition, Vector3 lightRotation, GTAObject lightObject)
{
	m_lightPosition = lightPosition;
	m_lightRotation = lightRotation; 
	m_lightObject = lightObject;

	m_initialLightPosition = m_lightPosition;
	m_initialLightRotation = m_lightRotation;
	
	//create object
	if (m_lightObject.objReference == 0) {
		if (m_lightObject.objHash == -1) {
			m_lightObject.objHash = GAMEPLAY::GET_HASH_KEY(m_lightObject.objName);
		}

		STREAMING::REQUEST_MODEL(m_lightObject.objHash);
		DWORD ticksStart = GetTickCount();
		while (!STREAMING::HAS_MODEL_LOADED(m_lightObject.objHash))
		{
			WAIT(0);
			if (GetTickCount() - ticksStart > 5000) {
				log_to_file("Failed to load GTAObject name:" + std::string(m_lightObject.objName) + " Hash:" + std::to_string(m_lightObject.objHash));
				set_status_text("Failed to load objects");
				return;
			}
		}

		int newObjectRef = OBJECT::CREATE_OBJECT(m_lightObject.objHash, m_lightPosition.x, m_lightPosition.y, m_lightPosition.z, true, true, false);
		m_lightObject.objReference = newObjectRef;
		ENTITY::SET_ENTITY_AS_MISSION_ENTITY(newObjectRef, true, true);

		ENTITY::SET_ENTITY_ROTATION(newObjectRef, m_lightRotation.x+180, 0, m_lightRotation.z, 2, true);
		log_to_file("Init light rotation pitch:" + std::to_string(m_lightRotation.x + 180) + " yaw:" + std::to_string(m_lightRotation.z));
	}
}

void StageLight::startTrackActor(Actor actor, int actorIndex)
{
	m_doTrackPed = true;
	m_trackPedId = actor.getActorPed();
	m_trackActorIndex = actorIndex;

	Vector3 currentActorPosition = ENTITY::GET_ENTITY_COORDS(m_trackPedId, true);
	m_trackOffset.x = m_lightPosition.x - currentActorPosition.x;
	m_trackOffset.y = m_lightPosition.y - currentActorPosition.y;
	m_trackOffset.z = m_lightPosition.z - currentActorPosition.z;

}

void StageLight::startTrackActor(Actor actor, int actorIndex, Vector3 trackOffset)
{
	m_doTrackPed = true;
	m_trackPedId = actor.getActorPed();
	m_trackActorIndex = actorIndex;


	m_trackOffset = trackOffset;
}

void StageLight::stopTrackActor()
{
	m_doTrackPed = false;
	m_trackPedId = 0;
	m_trackActorIndex = -1;
	m_trackOffset = Vector3();
}

int StageLight::getTrackedActorIndex()
{
	return m_trackActorIndex;
}

bool StageLight::isTrackingActor()
{
	return m_doTrackPed;
}

bool StageLight::hasFlicker()
{
	return m_hasFlicker;
}

void StageLight::removeLightObject()
{	//delete old object
	if (m_lightObject.objReference != 0) {
		OBJECT::DELETE_OBJECT(&m_lightObject.objReference);
	}
}

void StageLight::turnOff()
{
	if (m_lightObject.objReference != 0) {
		//ENTITY::SET_ENTITY_ALPHA(m_lightObject.objReference, 0, false);
		ENTITY::SET_ENTITY_VISIBLE(m_lightObject.objReference, false, false);
		//log_to_file("Light off");
	}
}

void StageLight::turnOn()
{
	if (m_lightObject.objReference != 0) {
		//ENTITY::SET_ENTITY_ALPHA(m_lightObject.objReference, 255, false);
		ENTITY::SET_ENTITY_VISIBLE(m_lightObject.objReference, true, false);
		//log_to_file("Light on");
	}
}

bool StageLight::hasRotation()
{
	return m_hasRotation;
}

void StageLight::setHasRotation(std::vector<StageLightRotation> rotationEvents)
{
	m_rotationEvents = rotationEvents;
	m_hasRotation = true;
}

std::vector<StageLightRotation> StageLight::getRotationEvents()
{
	return m_rotationEvents;
}

void StageLight::stopRotation()
{
	m_hasRotation = false;
}

bool StageLight::hasMovement()
{
	return m_hasMovement;
}

void StageLight::setHasMovement(std::vector<StageLightMovement> movementEvents)
{
	m_movementEvents = movementEvents;
	m_hasMovement = true;
}

std::vector<StageLightMovement> StageLight::getMovementEvents()
{
	return m_movementEvents;
}

void StageLight::stopMovement()
{
	m_hasMovement = false;
}

void StageLight::actionOnTick(DWORD tick, std::vector<Actor>& actors)
{
	if (m_doTrackPed) {
		Vector3 currentActorPosition = ENTITY::GET_ENTITY_COORDS(m_trackPedId, true);
		Vector3 newLightPosition;

		newLightPosition.x = currentActorPosition.x + m_trackOffset.x;
		newLightPosition.y = currentActorPosition.y + m_trackOffset.y;
		newLightPosition.z = currentActorPosition.z + m_trackOffset.z;

		if (newLightPosition.x != m_trackActorLastPos.x || newLightPosition.y != m_trackActorLastPos.y || newLightPosition.z != m_trackActorLastPos.z) {
			moveLight(newLightPosition);

			m_trackActorLastPos.x = newLightPosition.x;
			m_trackActorLastPos.y = newLightPosition.y;
			m_trackActorLastPos.z = newLightPosition.z;
		}


	}

	if (m_hasFlicker) {
		if (m_flickerStart == 0) {
			m_flickerStart = tick;
			m_flickerIndex = 0;
		}

		if (m_flickerIndex < m_flickerEvents.size()) {
			StageLightFlicker flickerEvent = m_flickerEvents[m_flickerIndex];
			
			if (tick - flickerEvent.length > m_flickerStart) {
				m_flickerIndex++;
				if (m_flickerIndex >= m_flickerEvents.size()) {
					m_flickerIndex = 0;
				}
				m_flickerStart = tick;
				flickerEvent = m_flickerEvents[m_flickerIndex];
				if (flickerEvent.isOn) {
					
					turnOn();
				}
				else {
					turnOff();
				}
			}
			else {
				//log_to_file("Light not changed " + std::to_string(tick) + " vs " + std::to_string(m_flickerStart + flickerEvent.length));
			}
		}
	}

	if (m_hasRotation) {
		if (m_rotationStart == 0) {
			m_rotationStart = tick;
			m_rotationIndex = 0;
		}

		if (m_rotationIndex < m_rotationEvents.size()) {
			StageLightRotation rotationEvent = m_rotationEvents[m_rotationIndex];

			if (tick - rotationEvent.length > m_rotationStart) {
				m_rotationIndex++;
				if (m_rotationIndex >= m_rotationEvents.size()) {
					m_rotationIndex = 0;
				}
				m_rotationStart = tick;
			}
			//always rotate light
			rotationEvent = m_rotationEvents[m_rotationIndex];
			Vector3 rotation;
			rotation.x = m_lightRotation.x;
			rotation.z = m_lightRotation.z;
			if (rotationEvent.hasPitch) {
				rotation.x += rotationEvent.pitch;
			}
			if (rotationEvent.hasYaw) {
				rotation.z += rotationEvent.yaw;
			}
			if (rotation.x != m_lightRotation.x || rotation.z != m_lightRotation.z) {
				rotateLight(rotation);
			}
		}
	}

	if (m_hasMovement) {
		if (m_movementStart == 0) {
			m_movementStart = tick;
			m_movementIndex = 0;
		}

		if (m_movementIndex < m_movementEvents.size()) {
			StageLightMovement movementEvent = m_movementEvents[m_movementIndex];

			if (tick - movementEvent.length > m_movementStart) {
				m_movementIndex++;
				if (m_movementIndex >= m_movementEvents.size()) {
					m_movementIndex = 0;
				}
				m_movementStart = tick;

			}
			//always move light
			movementEvent = m_movementEvents[m_movementIndex];

			Vector3 newPosition;
			newPosition.x = m_lightPosition.x + movementEvent.movementDelta.x;
			newPosition.y = m_lightPosition.y + movementEvent.movementDelta.y;
			newPosition.z = m_lightPosition.z + movementEvent.movementDelta.z;

			//if xyRatio, always use x value as basis
			if (movementEvent.hasxyRatio) {
				newPosition.y = m_lightPosition.y + movementEvent.movementDelta.x * movementEvent.xyRatio;
			}

			if (newPosition.x != m_lightPosition.x || newPosition.y != m_lightPosition.y || newPosition.z != m_lightPosition.z) {
				moveLight(newPosition);
			}
		}
	}


}

Vector3 StageLight::getLightPosition()
{
	return m_lightPosition;
}

Vector3 StageLight::getLightRotation()
{
	return  m_lightRotation;
}

GTAObject StageLight::getLightObject()
{
	return m_lightObject;
}

Vector3 StageLight::getTrackedActorOffset()
{
	return m_trackOffset;
}
