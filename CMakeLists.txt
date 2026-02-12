# Define o padrão C++23 como solicitado
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Informa ao compilador onde buscar os headers da My-lib
# O "../" serve para subir um nível, assumindo que a my-lib está ao lado da pasta do seu projeto
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../my-lib/include")