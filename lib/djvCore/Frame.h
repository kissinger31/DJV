// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Range.h>

namespace djv
{
    namespace Core
    {
        //! This namespace provides frame number functionality.
        namespace Frame
        {
            //! This type represents a frame number.
            typedef int64_t Number;

            //! This type represents a frame index.
            typedef int64_t Index;

            //! This value represents an invalid frame number.
            const int64_t invalid = std::numeric_limits<int64_t>::min();

            //! This value represents an invalid frame index.
            const int64_t invalidIndex = std::numeric_limits<int64_t>::min();

            //! This typedef provides a frame number range.
            typedef Core::Range::Range<Number> Range;

            //! This value represents an invalid frame number range.
            const Range invalidRange = Range(invalid, invalid);
            
            //! This class provides a sequence of frame numbers. A sequence is
            //! composed of multiple frame number ranges (e.g., 1-10,20-30).
            class Sequence
            {
            public:
                Sequence();
                explicit Sequence(Number);
                Sequence(Number, Number, size_t pad = 0);
                explicit Sequence(const Range& range, size_t pad = 0);
                explicit Sequence(const std::vector<Range>& ranges, size_t pad = 0);

                std::vector<Range> ranges;
                size_t pad = 0;

                bool isValid() const;
                bool contains(Index) const;
                size_t getSize() const;
                Number getFrame(Index) const;
                Index getIndex(Number) const;
                Index getLastIndex() const;

                //! \name Utilities
                ///@{
                
                //! Sort the sequence so that the frame numbers are in ascending order.
                void sort();
                
                bool merge(const Range&);
                
                ///@}

                bool operator == (const Sequence&) const;
                bool operator != (const Sequence&) const;
            };

            //! \name Utilities
            ///@{

            bool isValid(const Range&);

            void sort(Range&);

            ///@}

            //! \name Conversion
            ///@{

            std::vector<Number> toFrames(const Range&);
            std::vector<Number> toFrames(const Sequence&);
            Sequence fromFrames(const std::vector<Number>&);

            std::string toString(Number, size_t pad = 0);
            std::string toString(const Range&, size_t pad = 0);
            std::string toString(const Sequence&);

            //! Throws:
            //! - std::exception
            void fromString(const std::string&, Range&, size_t& pad);

            //! Throws:
            //! - std::exception
            void fromString(const std::string&, Sequence&);

            ///@}

        } // namespace Frame
    } // namespace Core

    std::ostream& operator << (std::ostream&, const Core::Frame::Sequence&);
    std::istream& operator >> (std::istream&, Core::Frame::Sequence&);

} // namespace djv

#include <djvCore/FrameInline.h>
