#ifndef MAPDRAWINTERFACE_DEFINES_H
#define MAPDRAWINTERFACE_DEFINES_H

#define CHECK_MAP   if (!m_config->hmap) { errorText = "[ERROR] Invalid map handle"; return false; }
#define CHECK_RSC   if (!m_config->hrsc) { errorText = "[ERROR] Invalid classificator handle"; return false; }

#define operate(func, ...) \
    operationResult = func(__VA_ARGS__); \
    if (operationResult <= 0) \
    { \
        errorText = QString("[ERROR] Function: [ %2() ] Line: [ %1 ] | Function [ %3() ] returned [ %4 ]").arg(QString::number(__LINE__), __FUNCTION__, #func, QString::number(operationResult));\
        return false;\
    }

#define createMapSiteObject(...) \
    m_config->objBuffer = mapCreateSiteObject(m_config->hmap, m_config->hsite, __VA_ARGS__); \
    if (!m_config->objBuffer) \
    { \
        errorText = "[ERROR] Create object"; \
        return false; \
    }

#define _createMapSiteObject() \
    m_config->objBuffer = mapCreateSiteObject(m_config->hmap, m_config->hsite); \
    if (!m_config->objBuffer) \
    { \
        errorText = "[ERROR] Create object"; \
        return false; \
    }

#endif // MAPDRAWINTERFACE_DEFINES_H
