#ifndef SKILL_CARD_EXCEPTION_HPP
#define SKILL_CARD_EXCEPTION_HPP

#include "NimonspoliException.hpp"
#include <string>

class InvalidSkillCardIndexException : public NimonspoliException
{
public:
    InvalidSkillCardIndexException(int index)
        : NimonspoliException(
              "Error Kartu Kemampuan: Index kartu " +
              std::to_string(index) +
              " tidak valid.")
    {
    }
};

class SkillCardOverflowNotFoundException : public NimonspoliException
{
public:
    SkillCardOverflowNotFoundException()
        : NimonspoliException(
              "Error Kartu Kemampuan: Tidak ada pemain yang sedang wajib membuang kartu.")
    {
    }
};

class SkillCardDropNotRequiredException : public NimonspoliException
{
public:
    SkillCardDropNotRequiredException()
        : NimonspoliException(
              "Error Kartu Kemampuan: Pemain tidak perlu membuang kartu karena jumlah kartu tidak melebihi batas.")
    {
    }
};

class SkillCardDiscardFailedException : public NimonspoliException
{
public:
    SkillCardDiscardFailedException()
        : NimonspoliException(
              "Error Kartu Kemampuan: Kartu gagal dibuang.")
    {
    }
};

class SkillCardInvalidActionException : public NimonspoliException
{
public:
    explicit SkillCardInvalidActionException(const std::string &message)
        : NimonspoliException("Error Kartu Kemampuan: " + message)
    {
    }
};

#endif