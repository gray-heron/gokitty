
#pragma once

#include <boost/any.hpp>
#include <torch/torch.h>

torch::Tensor TFV(float x, float y);
torch::Tensor TFS(float x);
float S(torch::Tensor t);

boost::any ParseValue(const std::type_info &type_id, std::string value);