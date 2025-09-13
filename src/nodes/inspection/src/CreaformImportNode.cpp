
#include "avs/nodes/inspection/CreaformImportNode.h"
#include <utility>

using avs::adapters::creaform::CreaformScan;

namespace avs { namespace nodes { namespace inspection {

avs::Result CreaformImportNode::Process(avs::graph::Context&){
    if(path_.empty()){
        return avs::Result::failure("CreaformImportNode: input path is empty");
    }
    auto [scan_ptr, res] = CreaformScan::LoadFromFile(path_);
    if(!res.ok) return res;
    scan_.reset(scan_ptr.release());
    return avs::Result::success();
}

}}}
