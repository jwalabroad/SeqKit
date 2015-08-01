#include "SnowTools/DBSnpFilter.h"
#include "SnowTools/SnowUtils.h"

#include <sstream> 

namespace SnowTools {

  DBSnpSite::DBSnpSite(const std::string& tchr, const std::string& pos, const std::string& rs, const std::string& ref, const std::string& alt) {
    
    // make the genomic region
    try { 
      chr = chrToNumber(tchr);
      pos1 = std::stoi(pos);
    } catch (...) {
      std::cerr << "DBSnpSite: Error trying to convert " << tchr << ":" << pos << " to number" << std::endl;
    }

    m_rs = rs;
    m_ref = ref;
    m_alt = alt;

    if (m_ref.length() == 0 || m_alt.length() == 0 || m_rs.length() == 0)
      std::cerr << "DBSnpSite: Is the VCF formated correctly for this entry? Ref " << m_ref << " ALT " << m_alt << " rs " << m_rs << std::endl;

    // insertion
    if (m_ref.length() == 1)
      pos2 = pos1 + 1;
    // deletion
    else
      pos2 = pos1 + m_ref.length() + 1;

    
  }

  DBSnpFilter::DBSnpFilter(const std::string& db) {
    
    // read in the file
    if (!read_access_test(db)) {
      std::cerr << std::endl << "**** Cannot read DBSnp database " << db << "   Expecting a VCF file" << std::endl;
      return;
    }

    // read it in
    igzstream in(db.c_str());
    if (!in) {
      std::cerr << std::endl << "**** Cannot read DBSnp database " << db << "   Expecting a VCF file" << std::endl;
      return;
    }

    std::string line;
    while (std::getline(in, line)) {

      if (line.find("#") != std::string::npos || line.length() == 0)
	continue;

      std::istringstream thisline(line);
      std::string val;
      int this_count = -1;
      std::string chr, pos, rs, ref, alt;
      while (std::getline(thisline, val, '\t')) {
	++this_count;
	switch (this_count) { 
	case 0: chr = val; break;
	case 1: pos = val; break;
	case 2: rs = val; break;
	case 3: ref = val; break;
	case 4: alt = val; break;
	}
      }

      DBSnpSite db(chr, pos, rs, ref, alt);

      // for now reject SNP sites
      if (ref.length() + alt.length() > 2)
	m_sites.add(db);
      
    }
    
    // build the tree
    m_sites.createTreeMap();

  }

  std::ostream& operator<<(std::ostream& out, const DBSnpFilter& d) {
    out << "DBSnpFilter with a total of " << AddCommas<size_t>(d.m_sites.size());
    return out;
  }

  std::ostream& operator<<(std::ostream& out, const DBSnpSite& d) {
    out << d.chr << ":" << d.pos1 << "-" << d.pos2 << "\t" << d.m_rs << " REF " << d.m_ref << " ALT " << d.m_alt;
  }

  bool DBSnpFilter::queryBreakpoint(BreakPoint& bp) {
    
    std::vector<int32_t> sub, que;
    GenomicRegion gr = bp.gr1;
    gr.pad(2);
    GRC subject(gr);
    GRC out = subject.findOverlaps(m_sites, sub, que, true); // true = ignore_strand
    
    if (que.size()) {
      bp.rs = m_sites[sub[0]].m_rs;
      for (auto& j : que) {
	bp.rs += m_sites[j].m_rs + "_";
      }
      bp.rs = cutLastChar(bp.rs);
      return true;
    }
    return false;
  }


}
