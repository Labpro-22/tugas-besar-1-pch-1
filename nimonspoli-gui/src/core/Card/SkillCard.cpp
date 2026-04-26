#include "SkillCard.hpp"

SkillCard::SkillCard() : Card(""), type(""), used(false) {}

SkillCard::SkillCard(const string &description, const string &type) : Card(description), type(type), used(false)
{
}

SkillCard::SkillCard(const string &type, const string &description, bool used) : Card(description), type(type), used(used)
{
}

SkillCard::~SkillCard()
{
}

bool SkillCard::isUsed() const
{
    return used;
}

void SkillCard::markUsed()
{
    used = true;
}

string SkillCard::getType() const
{
    return type;
}

std::string SkillCard::successMessage() const
{
    return getType() + " berhasil digunakan.";
}

void SkillCard::resetUsed() { used = false; }