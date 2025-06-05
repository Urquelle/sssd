#pragma once

#include <stdexcept>

#define TEST_ANNAHME(bedingung, meldung) \
    do { \
        if (!(bedingung)) { \
            throw std::runtime_error(std::string(__FILE__) + ":" + \
                                   std::to_string(__LINE__) + " " + \
                                   std::string(meldung)); \
        } \
    } while (false)

#define TEST_ANNAHME_GL(erwartet, erhalten, meldung) \
    do { \
        if ((erwartet) != (erhalten)) { \
            std::ostringstream oss; \
            oss << __FILE__ << ":" << __LINE__ << " " << (meldung) << " Erwartet: " \
                << (erwartet) << ", Erhalten: " << (erhalten); \
            throw std::runtime_error(oss.str()); \
        } \
    } while (false)

#define TEST_ANNAHME_NGL(erwartet, erhalten, meldung) \
    do { \
        if ((erwartet) == (erhalten)) { \
            std::ostringstream oss; \
            oss << __FILE__ << ":" << __LINE__ << " " << (meldung) << " Erwartet ungleich: " \
                << (erwartet) << ", Erhalten: " << (erhalten); \
            throw std::runtime_error(oss.str()); \
        } \
    } while (false)

#define TEST_ANNAHME_TXTGL(erwartet, erhalten, meldung) \
    do { \
        if (std::string(erwartet) != std::string(erhalten)) { \
            throw std::runtime_error(std::string(__FILE__) + ":" + \
                                   std::to_string(__LINE__) + " " + \
                                   std::string(meldung) + " Erwartet: " + \
                                   erwartet + ", Erhalten: " + erhalten); \
        } \
    } while (false)

