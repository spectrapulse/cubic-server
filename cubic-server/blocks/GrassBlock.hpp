#include <cstdint>
namespace Blocks {
typedef int32_t BlockId;

namespace GrassBlock {
namespace Properties {
enum class Snowy {
    TRUE,
    FALSE
};
}
BlockId toProtocol(Properties::Snowy snowy);
}

}
