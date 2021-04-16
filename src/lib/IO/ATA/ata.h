#ifndef ATA__H__
#define ATA__H__


namespace bio
{
    struct Buffer;
}

namespace ata
{
    void init();
    void io(bio::Buffer&);
    void irq();
} // namespace ata


#endif // !ATA__H__
