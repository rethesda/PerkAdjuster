#include "perkManipulator.h"

namespace PerkManipulation {
	bool Manipulator::Install()
	{
		SKSE::AllocTrampoline(14);
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<std::uintptr_t> target{ REL::ID(52527), 0xD7C };
		_getDescription = trampoline.write_call<5>(target.address(), &GetDescription);
		return true;
	}

	bool Manipulator::ChangeDescription(std::string a_new, std::string a_old, int a_match)
	{
		if (a_new.empty() || a_old.empty()) {
			return false;
		}
		if (descriptionMatches.contains(a_new) && descriptionMatches.at(a_old).second < a_match) {
			return false;
		}

		descriptionMatches[a_old] = std::make_pair(a_new, a_match);
		return true;
	}

	void Manipulator::PlaceNewPerk(RE::BGSPerk* a_newPerk, 
		RE::ActorValueInfo* a_targetValue, 
		int32_t a_targetIndex, 
		float a_x, float a_y, 
		std::vector<RE::BGSSkillPerkTreeNode*> a_parents, 
		std::vector<RE::BGSSkillPerkTreeNode*> a_children)
	{
		auto* newNode = new RE::BGSSkillPerkTreeNode(a_targetIndex, a_targetValue);
		newNode->perk = a_newPerk;
		newNode->perkGridX = std::floor(a_x);
		newNode->perkGridY = std::floor(a_y);
		newNode->horizontalPosition = a_x - newNode->perkGridX;
		newNode->verticalPosition = a_y - newNode->perkGridY;

		auto* target = a_targetValue->perkTree;

		for (auto* parent : a_parents) {
			newNode->parents.push_back(parent);
			parent->children.push_back(newNode);
		}

		for (auto* child : a_children) {
			newNode->children.push_back(child);
			child->parents.push_back(newNode);
		}

		a_targetValue->perkTree->children.push_back(newNode);
	}

	void Manipulator::GetDescription(RE::TESDescription* a_this, RE::BSString& a_out, RE::TESForm* a_parent, std::uint32_t a_fieldType)
	{
		_getDescription(a_this, a_out, a_parent, a_fieldType);

		std::string inString = a_out.c_str();
		if (!descriptionMatches.contains(inString)) return;

		a_out = descriptionMatches[inString].first;
	}
}