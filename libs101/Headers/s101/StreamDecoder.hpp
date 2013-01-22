#ifndef __LIBS101_STREAMDECODER_HPP
#define __LIBS101_STREAMDECODER_HPP

#include <vector>
#include "Byte.hpp"
#include "util/Crc16.hpp"

namespace libs101
{
    /**
     * Base class which decodes a S101 message.
     */
    template<typename ValueType = unsigned char>
    class StreamDecoder
    {
        typedef std::vector<ValueType> ByteVector;

    public:
        typedef ValueType value_type;
        typedef typename ByteVector::iterator iterator;
        typedef typename ByteVector::const_iterator const_iterator;
        typedef typename ByteVector::pointer pointer;
        typedef typename ByteVector::const_pointer const_pointer;
        typedef typename ByteVector::reference reference;
        typedef typename ByteVector::const_reference const_reference;
        typedef typename ByteVector::size_type size_type;

        /** Constructor */
        StreamDecoder();

        /** Destructor */
        ~StreamDecoder();

        /**
         * Reads n bytes from the provided input buffer. Each time a 
         * valid message has been decoded, the provided callback function will
         * be invoked with the decoded result. The result contains the decoded
         * data buffer.
         * @param first First item of the buffer to decode the data from.
         * @param last Last item of the buffer to decode the data from.
         * @param callback Callback function that will be called when a valid
         *      message has been decoded. The function must have the following 
         *      signature: (const_iterator, const_iterator, StateType)
         * @param state A user state that can be used to transfer any 
         *      kind of data to the callback function.
         * @note Each time a message has been decoded this method calls reset.
         */
        template<typename InputIterator, typename CallbackType, typename StateType>
        void read(InputIterator first, InputIterator last, CallbackType callback, StateType state);

        /**
         * Reads n bytes from the provided input buffer. Each time a 
         * valid message has been decoded, the provided callback function will
         * be invoked with the decoded result. The result contains the decoded
         * data buffer.
         * @param first First item of the buffer to decode the data from.
         * @param last Last item of the buffer to decode the data from.
         * @param callback Callback function that will be called when a valid
         *      message has been decoded. The function must have the following 
         *      signature: (const_iterator, const_iterator)
         * @note Each time a message has been decoded this method calls reset.
         */
        template<typename InputIterator, typename CallbackType>
        void read(InputIterator first, InputIterator last, CallbackType callback);

        /**
         * Decodes a single byte. If this is the last byte of a S101 message
         * the provided callback function will be invoked.
         * @param input The byte to decode.
         * @param callback Callback function that will be called when a valid
         *      message has been decoded. The function must have the following 
         *      signature: (const_iterator, const_iterator, StateType)
         * @param state A user state that can be used to transfer any 
         *      kind of data to the callback function.
         * @note Each time a message has been decoded this method calls reset.
         */
        template<typename InputType, typename CallbackType, typename StateType>
        void readByte(InputType input, CallbackType callback, StateType state);

        /**
         * Decodes a single byte. If this is the last byte of a S101 message
         * the provided callback function will be invoked.
         * @param input The byte to decode.
         * @param callback Callback function that will be called when a valid
         *      message has been decoded. The function must have the following 
         *      signature: (const_iterator, const_iterator)
         * @note Each time a message has been decoded this method calls reset.
         */
        template<typename InputType, typename CallbackType>
        void readByte(InputType input, CallbackType callback);

        /** Resets the current decoding buffer. */
        void reset();

    private:
        /**
         * This static method is used to invoke a callback which doesn't have a state parameter.
         * @param first Start of the buffer containing a decoded S101 message.
         * @param last End of the buffer.
         * @param callback The stateless callback, which only expects the data buffer.
         */
        template<typename CallbackType>
        static void invokeStatelessCallback(const_iterator first, const_iterator last, CallbackType callback);

        ByteVector m_bytes;
        bool m_escape;
        util::Crc16::value_type m_crc;
    };

    /**************************************************************************
     * Mandatory inline implementation                                        *
     **************************************************************************/

    template<typename ValueType>
    inline StreamDecoder<ValueType>::StreamDecoder()
        : m_escape(false)
        , m_crc(0xFFFF)
    {}

    template<typename ValueType>
    inline StreamDecoder<ValueType>::~StreamDecoder()
    {}

    template<typename ValueType>
    template<typename CallbackType>
    inline void StreamDecoder<ValueType>::invokeStatelessCallback(const_iterator first, const_iterator last, CallbackType callback)
    {
        callback(first, last);
    }

    template<typename ValueType>
    inline void StreamDecoder<ValueType>::reset()
    {
        m_bytes.clear();
        m_escape = false;
        m_crc = 0xFFFF;
    }

    template<typename ValueType>
    template<typename InputIterator, typename CallbackType, typename StateType>
    inline void StreamDecoder<ValueType>::read(InputIterator first, InputIterator last, CallbackType callback, StateType state)
    {
        for(; first != last; ++first)
            readByte(*first, callback, state);
    }

    template<typename ValueType>
    template<typename InputIterator, typename CallbackType>
    inline void StreamDecoder<ValueType>::read(InputIterator first, InputIterator last, CallbackType callback)
    {
        for(; first != last; ++first)
            readByte(*first, callback);
    }

    template<typename ValueType>
    template<typename InputType, typename CallbackType>
    inline void StreamDecoder<ValueType>::readByte(InputType input, CallbackType callback)
    {
        typedef void (*CallbackBindType)(const_iterator, const_iterator, CallbackType);

        readByte<InputType, CallbackBindType, CallbackType>(input, invokeStatelessCallback, callback);
    }

    template<typename ValueType>
    template<typename InputType, typename CallbackType, typename StateType>
    inline void StreamDecoder<ValueType>::readByte(InputType input, CallbackType callback, StateType state)
    {
        value_type const byte = static_cast<value_type>(input);

        if (m_escape)
        {
            m_escape = false;
            m_bytes.push_back(byte ^ Byte::XOR);
            m_crc = util::Crc16::add(m_crc, byte ^ Byte::XOR);
        }
        else
        {
            switch(byte)
            {
                case Byte::BoF:
                    reset();
                    break;

                case Byte::EoF:
                    if (m_crc == 0xF0B8 && m_bytes.size() > 1)
                        callback(m_bytes.begin(), m_bytes.end() - 2, state);

                    reset();
                    break;

                case Byte::CE:
                    m_escape = true;
                    break;

                default:
                    m_bytes.push_back(byte);
                    m_crc = util::Crc16::add(m_crc, byte);
                    break;
            }
        }
    }
}

#endif  // __LIBS101_STREAMDECODER_HPP
