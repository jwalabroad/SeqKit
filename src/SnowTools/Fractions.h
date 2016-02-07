#ifndef SNOWMAN_FRACTIONS_H__
#define SNOWMAN_FRACTIONS_H__

#include "SnowTools/GenomicRegionCollection.h"
#include <string>

namespace SnowTools {

  /** @brief Extension of GenomicRegion with a fraction of reads to keep on an interval
   * 
   * Used in conjunction with Fractions and used for selectively sub-sampling BAM files.
   */
class FracRegion : public SnowTools::GenomicRegion {

 public:
  
  FracRegion() {}

  FracRegion(const std::string& c, const std::string& p1, const std::string& p2, bam_hdr_t * h, const std::string& f);

  friend std::ostream& operator<<(std::ostream& out, const FracRegion& f);

  double frac;
};

  /** @brief Genomic intervals and associated sampling fractions
   */
class Fractions {

 public:
  
  Fractions() {}

  size_t size() const;

  void readFromBed(const std::string& file, bam_hdr_t * h) ;

  SnowTools::GenomicRegionCollection<FracRegion> m_frc;

 private:

};

}

#endif
