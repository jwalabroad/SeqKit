#ifndef SNOWTOOLS_BAM_WRITER_H__
#define SNOWTOOLS_BAM_WRITER_H__

#include <cassert>
#include <memory>

namespace SnowTools {

/** Walk along a BAM or along BAM regions and stream in/out reads
 */
class BamWriter  {

 public:

  /** Construct a new BamWriter for writing a BAM file 
   * @param f Name of BAM file to write
   * @note Immediately calls OpenWriteBam.
   */
  BamWriter(const std::string& f);

  /** Construct an empty BamWriter */
  BamWriter() {}

  /** Destroy a BamWriter and close all connections to the BAM 
   * 
   * Calling the destructor will take care of all of the C-style dealloc
   * calls required within HTSlib to close a BAM or SAM file. 
   */
  ~BamWriter() {}

  /** Close a BAM file explitily. This is required before indexing with makeIndex.
   * @note If not called, BAM will close properly on object destruction
   * @exception Throws a runtime_error if BAM already closed or was never opened
   */
  void CloseBam();

  /** Create the index file for the output bam in BAI format.
   *
   * This will make a call to HTSlib bam_index_build for the output file. 
   * @exception Throws a runtime_error if sam_index_build2 exits with < 0 status
   */
  void makeIndex() const;

  /** Print out some basic info about this walker, 
   * including Minz0iRules
   */
  friend std::ostream& operator<<(std::ostream& out, const BamWriter& b);

  /** Open a BAM file for streaming out.
   * @param bam Path to the output BAM file
   * @exception Throws a runtime_error if cannot write BAM
   * @note Calling this function will immediately write the BAM with its header
   */
  void OpenWriteBam(const std::string& bam);

  /** Write an alignment to the output BAM file 
   * @param r The BamRead to save
   * @exception Throws a runtime_error if cannot write alignment
   */
  void writeAlignment(BamRead &r);

  /** Set a flag to say if we should print reads to stdout */
  void setStdout();

  /** Set a flag to say if we should print reads to CRAM format
   * @param out Output CRAM file to write to
   * @param ref File with the reference genome used for compression
   */
  void setCram(const std::string& ref);

  /** If set to true, will print header in output 
   * @param val Set whether to print the hader
   */
  void setPrintHeader(bool val) { 
    m_print_header = val;
  }
  
  /** Return a pointer to the BAM header */
  bam_hdr_t * header() const { return br.get(); };

  /** Explicitly provide the output BAM a header. 
   * 
   * This will create a copy of the bam_hdr_t for this BamWriter.
   */
  void SetWriteHeader(bam_hdr_t* hdr);

 protected:

  // path to output file
  std::string m_out; 

  // for stdout mode, print header?
  bool m_print_header = false;

  // open m_out, true if success
  void __open_BAM_for_writing();
  
  // hts
  std::shared_ptr<BGZF> fp;
  std::shared_ptr<hts_idx_t> idx;
  std::shared_ptr<hts_itr_t> hts_itr;
  std::shared_ptr<bam_hdr_t> br;
  std::shared_ptr<bam_hdr_t> hdr_write;

  std::shared_ptr<htsFile> fop;

  // which tags to strip
  std::vector<std::string> m_tag_list;

};


}
#endif 


