#include "testchip_tsi.h"

testchip_tsi_t::testchip_tsi_t(int argc, char** argv, bool can_have_loadmem) : tsi_t(argc, argv)
{
  has_loadmem = false;
  init_writes = std::vector<std::pair<uint64_t, uint32_t>>();

  std::vector<std::string> args(argv + 1, argv + argc);
  for (auto& arg : args) {
    if (arg.find("+loadmem=") == 0)
      has_loadmem = can_have_loadmem;
    if (arg.find("+init_write=") == 0) {
      auto d = arg.find(":");
      if (d == std::string::npos) {
	throw std::invalid_argument("Improperly formatted +init_write argument");
      }
      uint64_t addr = strtoull(arg.substr(12, d - 12).c_str(), 0, 16);
      uint32_t val = strtoull(arg.substr(d + 1).c_str(), 0, 16);

      init_writes.push_back(std::make_pair(addr, val));
    }
  }
}


void testchip_tsi_t::write_chunk(addr_t taddr, size_t nbytes, const void* src)
{
  if (is_loadmem) {
    load_mem_write(taddr, nbytes, src);
  } else {
    tsi_t::write_chunk(taddr, nbytes, src);
  }
}

void testchip_tsi_t::read_chunk(addr_t taddr, size_t nbytes, void* dst)
{
  if (is_loadmem) {
    load_mem_read(taddr, nbytes, dst);
  } else {
    tsi_t::read_chunk(taddr, nbytes, dst);
  }
}

void testchip_tsi_t::reset()
{
  for (auto p : init_writes) {
    write_chunk(p.first, sizeof(uint32_t), &p.second);
  }
  tsi_t::reset();
}
