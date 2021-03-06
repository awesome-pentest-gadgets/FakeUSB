/*
 * This file is not part of original USBProxy.
 * Sub-class of Command. 
 * Moves the mouse of x pixels horizontally and y pixels vertically while clicking a button. 
 * Positive numbers are right/down movements, negative numbers are left/up movements. Params are button to click (Left, Right, Middle), x pixels and y pixels.
 *
 * Author: dibbidouble & Skazza
 */

#include "CommandMoveAndClick.h"

CommandMoveAndClick::CommandMoveAndClick() : Command() {}

CommandMoveAndClick::~CommandMoveAndClick() {}

std::list<std::pair<__u8 *, __u64>> * CommandMoveAndClick::preparePayLoad(std::vector<std::string> * params, __u16 maxPacketSize) {
	std::list<std::pair<__u8 *, __u64>> * payLoad = new std::list<std::pair<__u8 *, __u64>>;

	__u8 button = findButton(params->at(0).at(0));
	int hMovement = std::stoi(params->at(1));
	int vMovement = std::stoi(params->at(2));
	bool isHPositive = (hMovement >= 0);
	bool isVPositive = (vMovement >= 0);

	hMovement = std::abs(hMovement);
	vMovement = std::abs(vMovement);

	int nPackets = std::max(hMovement, vMovement);

	for(int i = 0; i < nPackets; i += 0x7f) {
		__u8 * packetMovement = (__u8 *) calloc(maxPacketSize, sizeof(__u8));
		packetMovement[0x00] = button;

		if(hMovement > 0) {
			int minHMovement = std::min(hMovement, 0x7f);
			packetMovement[0x01] = (isHPositive) ? minHMovement : (0xff - minHMovement + 0x01);
			hMovement -= minHMovement;
		}

		payLoad->push_back(std::pair<__u8 *, __u64>(packetMovement, maxPacketSize));
	}

	for(std::list<std::pair<__u8 *, __u64>>::iterator it = payLoad->begin(); it != payLoad->end(); ++it) {
		if(vMovement > 0) {
			int minVMovement = std::min(vMovement, 0x7f);
			(*it).first[0x02] = (isVPositive) ? minVMovement : (0xff - minVMovement + 0x01);
			vMovement -= minVMovement;
		}
	}

	__u8 * packet = (__u8 *) calloc(maxPacketSize, sizeof(__u8));
	packet[0x00] = button;
	payLoad->push_front(std::pair<__u8 *, __u64>(packet, maxPacketSize));

	packet = (__u8 *) calloc(maxPacketSize, sizeof(__u8));
	payLoad->push_back(std::pair<__u8 *, __u64>(packet, maxPacketSize));

	return payLoad;
}

std::vector<std::string> * CommandMoveAndClick::parseParams(const std::string &paramString) {
	std::regex paramRegex("^(R|L|M) (-?\\d+),(-?\\d+)$", std::regex_constants::icase);
	std::smatch matches; std::regex_search(paramString, matches, paramRegex);

	if(!matches[1].str().empty() && !matches[2].str().empty() && !matches[3].str().empty()) {
		std::vector<std::string> * paramVector = new std::vector<std::string>;
		paramVector->push_back(matches[1].str());
		paramVector->push_back(matches[2].str());
		paramVector->push_back(matches[3].str());

		return paramVector;
	}

	return NULL;
}


std::list<std::pair<__u8 *, __u64>> * CommandMoveAndClick::execute(const std::string &paramString, __u16 maxPacketSize) {
	std::vector<std::string> * paramList = this->parseParams(paramString);

	if(paramList) {
		std::list<std::pair<__u8 *, __u64>> * payLoad = this->preparePayLoad(paramList, maxPacketSize);
		delete(paramList);

		return payLoad;
	}

	return new std::list<std::pair<__u8 *, __u64>>;
}

/* Autoregisters the class into the CommandFactory */
const bool hasRegistered = CommandFactory::getInstance()->registerClass("MOVE_AND_CLICK", &CommandMoveAndClick::createInstance);


