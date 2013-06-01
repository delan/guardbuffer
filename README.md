GuardBuffer
===========

Buffers and outputs input in regular blocks.

Windows: copying raw partitions over a network
----------------------------------------------

To copy a partition on one computer to a file on another is simple. Let's assume
that 64 KiB is a 'sweet spot' block size where performance is maximised.

	dd bs=65536 if=\\?\Device\HarddiskX\PartitionY of=\\dest\path\file

Great, but copying directly from partition to partition is trickier. Very few
programs, `dd` included, support interacting with block device files and they
cannot be accessed over a network via sharing.

A potential solution, with the first command run on the source, and the other
on the destination:

	dd bs=65536 if=\\?\Device\HarddiskVolumeA | ncat -l 65432 --send-only
	ncat source 65432 --recv-only | dd bs=65536 of=\\?\Device\Hard...B

Strangely, the second `dd` isn't patient enough to wait for a full 64 KiB block,
and ends up attempting to write partial blocks. On Windows, writes to block
devices that are not a multiple of the sector size fail. This causes `dd` to
retry writing the partial blocks, padded with zeros, resulting in corruption.

The solution is simple, a buffer is needed to ensure that `dd` only ever
receives data in chunks of the block size, regardless of how they arrive from
the network.

	dd bs=65536 if=\\?\Device\HarddiskVolumeA | ncat -l 65432 --send-only
	ncat source 65432 --recv-only | gb | dd bs=65536 of=\\?\Device\Hard...B

In addition to this, `gb` also provides indications of time, progress and speed.
