/**
* I haven't yet gotten around to adding the licensing header.
*
* This file and the counterpart auth-tag.cpp are additions to
* the ndn-cxx library for use in NMSU's tag based access control
* mechanism.
*
* @author Ray Stubbs [stubbs.ray@gmail.com]
**/
#ifndef AUTH_TAG_HPP
#define AUTH_TAG_HPP

#include "common.hpp"
#include "encoding/block.hpp"
#include "signature.hpp"
#include "ndn-cxx/security/validity-period.hpp"

namespace ndn
{
    class AuthTag
    {
        private:
            mutable Block m_wire;
            Name          m_prefix;
            uint8_t       m_access_level;
            uint64_t      m_route_hash;
            KeyLocator    m_consumer_locator;
            Signature     m_signature;

            void onChanged()
            {
                m_wire.reset();
            }
        public:
            AuthTag( const Block& wire );
            AuthTag( uint8_t access_level );
            AuthTag();
            template<encoding::Tag TAG>
            size_t wireEncode( EncodingImpl< TAG > &encoder ) const;
            const Block& wireEncode() const;
            void wireDecode( const Block& wire );
            bool hasWire();

            const Name& getPrefix() const
            {
              return m_prefix;
            };

            void setPrefix( const Name& prefix )
            {
              m_prefix = prefix;
            };

            uint8_t getAccessLevel() const
            {
                return m_access_level;
            };

            void setAccessLevel( uint8_t access_level )
            {
                onChanged();
                m_access_level = access_level;
            };

            uint64_t getRouteHash() const
            {
              return m_route_hash;
            };

            void setRouteHash( uint64_t hash )
            {
              m_route_hash = hash;
            }

            const KeyLocator& getConsumerLocator() const
            {
              return m_consumer_locator;
            }

            void setConsumerLocator( const KeyLocator& locator )
            {
              m_consumer_locator = locator;
            }

            time::system_clock::TimePoint getActivationTime() const
            {
                return m_signature.getSignatureInfo().getValidityPeriod().getPeriod().first;
            };

            void setActivationTime( const time::system_clock::TimePoint& tp )
            {
                onChanged();
                security::ValidityPeriod pd;
                try
                {
                    pd = m_signature.getSignatureInfo().getValidityPeriod();
                }
                catch( ... )
                {
                    pd = security::ValidityPeriod();
                }
                pd.setPeriod( tp, pd.getPeriod().second );
                SignatureInfo info( m_signature.getSignatureInfo() );
                info.setValidityPeriod( pd );
                m_signature.setInfo( info );
            };

            time::system_clock::TimePoint getExpirationTime() const
            {
                return m_signature.getSignatureInfo().getValidityPeriod().getPeriod().second;
            };

            void setExpirationTime( const time::system_clock::TimePoint& tp )
            {
                onChanged();
                security::ValidityPeriod pd;
                try
                {
                    pd = m_signature.getSignatureInfo().getValidityPeriod();
                }
                catch( ... )
                {
                    pd = security::ValidityPeriod();
                }

                pd.setPeriod( pd.getPeriod().first, tp );
                SignatureInfo info( m_signature.getSignatureInfo() );
                info.setValidityPeriod( pd );
                m_signature.setInfo( info );
            };

            bool isExpired() const
            {
                // if no validity period set then does not expire
                try
                {
                    auto now = time::system_clock::now();
                    return now < getActivationTime() || now > getExpirationTime();
                }
                catch( ... )
                {
                    return false;
                }
            };

            const KeyLocator& getKeyLocator() const
            {
                return m_signature.getKeyLocator();
            };

            void setKeyLocator( const KeyLocator& locator )
            {
                onChanged();
                SignatureInfo info(m_signature.getSignatureInfo() );
                info.setKeyLocator( locator );
                m_signature.setInfo( info );
            };

            const Signature& getSignature() const
            {
                return m_signature;
            };

            void setSignature( const Signature& sig )
            {
                onChanged();
                m_signature = sig;

            };

            void setSignatureValue( const Block& sig_value )
            {
              m_signature.setValue( sig_value );
            }

            bool operator==( const AuthTag& other ) const
            {
                return wireEncode() == other.wireEncode();
            };

            operator bool () const
            {
                return (bool)m_access_level;
            };

    };


}

#endif // AUTH_TAG_HPP
