<?php
/**
 * Auto generated from adv_protocol.proto at 2016-03-12 11:20:50
 *
 * com.adv.msg package
 */

namespace Com\Adv\Msg {
/**
 * UserKeepAliveRequest message
 */
class UserKeepAliveRequest extends \ProtobufMessage
{
    /* Field index constants */
    const CONDID = 1;
    const DEVICETYPE = 2;
    const DEVICEID = 3;
    const UID = 4;
    const VERSION = 5;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::CONDID => array(
            'name' => 'condid',
            'required' => true,
            'type' => 5,
        ),
        self::DEVICETYPE => array(
            'name' => 'devicetype',
            'required' => true,
            'type' => 5,
        ),
        self::DEVICEID => array(
            'name' => 'deviceid',
            'required' => false,
            'type' => 7,
        ),
        self::UID => array(
            'name' => 'uid',
            'required' => false,
            'type' => 5,
        ),
        self::VERSION => array(
            'name' => 'version',
            'required' => false,
            'type' => 5,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::CONDID] = null;
        $this->values[self::DEVICETYPE] = null;
        $this->values[self::DEVICEID] = null;
        $this->values[self::UID] = null;
        $this->values[self::VERSION] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'condid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setCondid($value)
    {
        return $this->set(self::CONDID, $value);
    }

    /**
     * Returns value of 'condid' property
     *
     * @return int
     */
    public function getCondid()
    {
        return $this->get(self::CONDID);
    }

    /**
     * Sets value of 'devicetype' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setDevicetype($value)
    {
        return $this->set(self::DEVICETYPE, $value);
    }

    /**
     * Returns value of 'devicetype' property
     *
     * @return int
     */
    public function getDevicetype()
    {
        return $this->get(self::DEVICETYPE);
    }

    /**
     * Sets value of 'deviceid' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setDeviceid($value)
    {
        return $this->set(self::DEVICEID, $value);
    }

    /**
     * Returns value of 'deviceid' property
     *
     * @return string
     */
    public function getDeviceid()
    {
        return $this->get(self::DEVICEID);
    }

    /**
     * Sets value of 'uid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setUid($value)
    {
        return $this->set(self::UID, $value);
    }

    /**
     * Returns value of 'uid' property
     *
     * @return int
     */
    public function getUid()
    {
        return $this->get(self::UID);
    }

    /**
     * Sets value of 'version' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setVersion($value)
    {
        return $this->set(self::VERSION, $value);
    }

    /**
     * Returns value of 'version' property
     *
     * @return int
     */
    public function getVersion()
    {
        return $this->get(self::VERSION);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * LoginRequest message
 */
class LoginRequest extends \ProtobufMessage
{
    /* Field index constants */
    const TOKEN = 1;
    const MAGICNUM = 2;
    const STATUS = 3;
    const DEVICETYPE = 4;
    const DEVICEID = 6;
    const DESCRIPTION = 7;
    const CONDID = 8;
    const LOGINSEQ = 9;
    const ACCOUNTTYPE = 10;
    const ACCOUNT = 11;
    const PASSWD = 12;
    const VERSION = 13;
    const NICKNAME = 14;
    const GENDER = 15;
    const PROVINCE = 16;
    const CITY = 17;
    const COUNTRY = 18;
    const AVATAR = 19;
    const DESC = 20;
    const EMAIL = 21;
    const MOBILE = 22;
    const TIME = 23;
    const UID = 24;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::TOKEN => array(
            'name' => 'token',
            'required' => true,
            'type' => 7,
        ),
        self::MAGICNUM => array(
            'name' => 'magicnum',
            'required' => false,
            'type' => 5,
        ),
        self::STATUS => array(
            'name' => 'status',
            'required' => false,
            'type' => 5,
        ),
        self::DEVICETYPE => array(
            'name' => 'devicetype',
            'required' => true,
            'type' => 5,
        ),
        self::DEVICEID => array(
            'name' => 'deviceid',
            'required' => true,
            'type' => 7,
        ),
        self::DESCRIPTION => array(
            'name' => 'description',
            'required' => false,
            'type' => 7,
        ),
        self::CONDID => array(
            'name' => 'condid',
            'required' => true,
            'type' => 5,
        ),
        self::LOGINSEQ => array(
            'name' => 'loginseq',
            'required' => false,
            'type' => 5,
        ),
        self::ACCOUNTTYPE => array(
            'name' => 'accounttype',
            'required' => false,
            'type' => 5,
        ),
        self::ACCOUNT => array(
            'name' => 'account',
            'required' => false,
            'type' => 7,
        ),
        self::PASSWD => array(
            'name' => 'passwd',
            'required' => false,
            'type' => 7,
        ),
        self::VERSION => array(
            'name' => 'version',
            'required' => false,
            'type' => 5,
        ),
        self::NICKNAME => array(
            'name' => 'nickname',
            'required' => false,
            'type' => 7,
        ),
        self::GENDER => array(
            'name' => 'gender',
            'required' => false,
            'type' => 5,
        ),
        self::PROVINCE => array(
            'name' => 'province',
            'required' => false,
            'type' => 7,
        ),
        self::CITY => array(
            'name' => 'city',
            'required' => false,
            'type' => 7,
        ),
        self::COUNTRY => array(
            'name' => 'country',
            'required' => false,
            'type' => 7,
        ),
        self::AVATAR => array(
            'name' => 'avatar',
            'required' => false,
            'type' => 7,
        ),
        self::DESC => array(
            'name' => 'desc',
            'required' => false,
            'type' => 7,
        ),
        self::EMAIL => array(
            'name' => 'email',
            'required' => false,
            'type' => 7,
        ),
        self::MOBILE => array(
            'name' => 'mobile',
            'required' => false,
            'type' => 7,
        ),
        self::TIME => array(
            'name' => 'time',
            'required' => false,
            'type' => 5,
        ),
        self::UID => array(
            'name' => 'uid',
            'required' => false,
            'type' => 5,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::TOKEN] = null;
        $this->values[self::MAGICNUM] = null;
        $this->values[self::STATUS] = null;
        $this->values[self::DEVICETYPE] = null;
        $this->values[self::DEVICEID] = null;
        $this->values[self::DESCRIPTION] = null;
        $this->values[self::CONDID] = null;
        $this->values[self::LOGINSEQ] = null;
        $this->values[self::ACCOUNTTYPE] = null;
        $this->values[self::ACCOUNT] = null;
        $this->values[self::PASSWD] = null;
        $this->values[self::VERSION] = null;
        $this->values[self::NICKNAME] = null;
        $this->values[self::GENDER] = null;
        $this->values[self::PROVINCE] = null;
        $this->values[self::CITY] = null;
        $this->values[self::COUNTRY] = null;
        $this->values[self::AVATAR] = null;
        $this->values[self::DESC] = null;
        $this->values[self::EMAIL] = null;
        $this->values[self::MOBILE] = null;
        $this->values[self::TIME] = null;
        $this->values[self::UID] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'token' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setToken($value)
    {
        return $this->set(self::TOKEN, $value);
    }

    /**
     * Returns value of 'token' property
     *
     * @return string
     */
    public function getToken()
    {
        return $this->get(self::TOKEN);
    }

    /**
     * Sets value of 'magicnum' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setMagicnum($value)
    {
        return $this->set(self::MAGICNUM, $value);
    }

    /**
     * Returns value of 'magicnum' property
     *
     * @return int
     */
    public function getMagicnum()
    {
        return $this->get(self::MAGICNUM);
    }

    /**
     * Sets value of 'status' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setStatus($value)
    {
        return $this->set(self::STATUS, $value);
    }

    /**
     * Returns value of 'status' property
     *
     * @return int
     */
    public function getStatus()
    {
        return $this->get(self::STATUS);
    }

    /**
     * Sets value of 'devicetype' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setDevicetype($value)
    {
        return $this->set(self::DEVICETYPE, $value);
    }

    /**
     * Returns value of 'devicetype' property
     *
     * @return int
     */
    public function getDevicetype()
    {
        return $this->get(self::DEVICETYPE);
    }

    /**
     * Sets value of 'deviceid' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setDeviceid($value)
    {
        return $this->set(self::DEVICEID, $value);
    }

    /**
     * Returns value of 'deviceid' property
     *
     * @return string
     */
    public function getDeviceid()
    {
        return $this->get(self::DEVICEID);
    }

    /**
     * Sets value of 'description' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setDescription($value)
    {
        return $this->set(self::DESCRIPTION, $value);
    }

    /**
     * Returns value of 'description' property
     *
     * @return string
     */
    public function getDescription()
    {
        return $this->get(self::DESCRIPTION);
    }

    /**
     * Sets value of 'condid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setCondid($value)
    {
        return $this->set(self::CONDID, $value);
    }

    /**
     * Returns value of 'condid' property
     *
     * @return int
     */
    public function getCondid()
    {
        return $this->get(self::CONDID);
    }

    /**
     * Sets value of 'loginseq' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setLoginseq($value)
    {
        return $this->set(self::LOGINSEQ, $value);
    }

    /**
     * Returns value of 'loginseq' property
     *
     * @return int
     */
    public function getLoginseq()
    {
        return $this->get(self::LOGINSEQ);
    }

    /**
     * Sets value of 'accounttype' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setAccounttype($value)
    {
        return $this->set(self::ACCOUNTTYPE, $value);
    }

    /**
     * Returns value of 'accounttype' property
     *
     * @return int
     */
    public function getAccounttype()
    {
        return $this->get(self::ACCOUNTTYPE);
    }

    /**
     * Sets value of 'account' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setAccount($value)
    {
        return $this->set(self::ACCOUNT, $value);
    }

    /**
     * Returns value of 'account' property
     *
     * @return string
     */
    public function getAccount()
    {
        return $this->get(self::ACCOUNT);
    }

    /**
     * Sets value of 'passwd' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setPasswd($value)
    {
        return $this->set(self::PASSWD, $value);
    }

    /**
     * Returns value of 'passwd' property
     *
     * @return string
     */
    public function getPasswd()
    {
        return $this->get(self::PASSWD);
    }

    /**
     * Sets value of 'version' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setVersion($value)
    {
        return $this->set(self::VERSION, $value);
    }

    /**
     * Returns value of 'version' property
     *
     * @return int
     */
    public function getVersion()
    {
        return $this->get(self::VERSION);
    }

    /**
     * Sets value of 'nickname' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setNickname($value)
    {
        return $this->set(self::NICKNAME, $value);
    }

    /**
     * Returns value of 'nickname' property
     *
     * @return string
     */
    public function getNickname()
    {
        return $this->get(self::NICKNAME);
    }

    /**
     * Sets value of 'gender' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setGender($value)
    {
        return $this->set(self::GENDER, $value);
    }

    /**
     * Returns value of 'gender' property
     *
     * @return int
     */
    public function getGender()
    {
        return $this->get(self::GENDER);
    }

    /**
     * Sets value of 'province' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setProvince($value)
    {
        return $this->set(self::PROVINCE, $value);
    }

    /**
     * Returns value of 'province' property
     *
     * @return string
     */
    public function getProvince()
    {
        return $this->get(self::PROVINCE);
    }

    /**
     * Sets value of 'city' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setCity($value)
    {
        return $this->set(self::CITY, $value);
    }

    /**
     * Returns value of 'city' property
     *
     * @return string
     */
    public function getCity()
    {
        return $this->get(self::CITY);
    }

    /**
     * Sets value of 'country' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setCountry($value)
    {
        return $this->set(self::COUNTRY, $value);
    }

    /**
     * Returns value of 'country' property
     *
     * @return string
     */
    public function getCountry()
    {
        return $this->get(self::COUNTRY);
    }

    /**
     * Sets value of 'avatar' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setAvatar($value)
    {
        return $this->set(self::AVATAR, $value);
    }

    /**
     * Returns value of 'avatar' property
     *
     * @return string
     */
    public function getAvatar()
    {
        return $this->get(self::AVATAR);
    }

    /**
     * Sets value of 'desc' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setDesc($value)
    {
        return $this->set(self::DESC, $value);
    }

    /**
     * Returns value of 'desc' property
     *
     * @return string
     */
    public function getDesc()
    {
        return $this->get(self::DESC);
    }

    /**
     * Sets value of 'email' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setEmail($value)
    {
        return $this->set(self::EMAIL, $value);
    }

    /**
     * Returns value of 'email' property
     *
     * @return string
     */
    public function getEmail()
    {
        return $this->get(self::EMAIL);
    }

    /**
     * Sets value of 'mobile' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setMobile($value)
    {
        return $this->set(self::MOBILE, $value);
    }

    /**
     * Returns value of 'mobile' property
     *
     * @return string
     */
    public function getMobile()
    {
        return $this->get(self::MOBILE);
    }

    /**
     * Sets value of 'time' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTime($value)
    {
        return $this->set(self::TIME, $value);
    }

    /**
     * Returns value of 'time' property
     *
     * @return int
     */
    public function getTime()
    {
        return $this->get(self::TIME);
    }

    /**
     * Sets value of 'uid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setUid($value)
    {
        return $this->set(self::UID, $value);
    }

    /**
     * Returns value of 'uid' property
     *
     * @return int
     */
    public function getUid()
    {
        return $this->get(self::UID);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * LoginResult message
 */
class LoginResult extends \ProtobufMessage
{
    /* Field index constants */
    const RETBASE = 1;
    const KEY = 2;
    const UID = 3;
    const LOGINSEQ = 4;
    const ACCOUNTTYPE = 5;
    const TIME = 6;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::RETBASE => array(
            'name' => 'retbase',
            'required' => true,
            'type' => '\Com\Adv\Msg\RetBase'
        ),
        self::KEY => array(
            'name' => 'key',
            'required' => false,
            'type' => 7,
        ),
        self::UID => array(
            'name' => 'uid',
            'required' => false,
            'type' => 5,
        ),
        self::LOGINSEQ => array(
            'name' => 'loginseq',
            'required' => false,
            'type' => 5,
        ),
        self::ACCOUNTTYPE => array(
            'name' => 'accounttype',
            'required' => false,
            'type' => 5,
        ),
        self::TIME => array(
            'name' => 'time',
            'required' => false,
            'type' => 5,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::RETBASE] = null;
        $this->values[self::KEY] = null;
        $this->values[self::UID] = null;
        $this->values[self::LOGINSEQ] = null;
        $this->values[self::ACCOUNTTYPE] = null;
        $this->values[self::TIME] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'retbase' property
     *
     * @param \Com\Adv\Msg\RetBase $value Property value
     *
     * @return null
     */
    public function setRetbase(\Com\Adv\Msg\RetBase $value)
    {
        return $this->set(self::RETBASE, $value);
    }

    /**
     * Returns value of 'retbase' property
     *
     * @return \Com\Adv\Msg\RetBase
     */
    public function getRetbase()
    {
        return $this->get(self::RETBASE);
    }

    /**
     * Sets value of 'key' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setKey($value)
    {
        return $this->set(self::KEY, $value);
    }

    /**
     * Returns value of 'key' property
     *
     * @return string
     */
    public function getKey()
    {
        return $this->get(self::KEY);
    }

    /**
     * Sets value of 'uid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setUid($value)
    {
        return $this->set(self::UID, $value);
    }

    /**
     * Returns value of 'uid' property
     *
     * @return int
     */
    public function getUid()
    {
        return $this->get(self::UID);
    }

    /**
     * Sets value of 'loginseq' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setLoginseq($value)
    {
        return $this->set(self::LOGINSEQ, $value);
    }

    /**
     * Returns value of 'loginseq' property
     *
     * @return int
     */
    public function getLoginseq()
    {
        return $this->get(self::LOGINSEQ);
    }

    /**
     * Sets value of 'accounttype' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setAccounttype($value)
    {
        return $this->set(self::ACCOUNTTYPE, $value);
    }

    /**
     * Returns value of 'accounttype' property
     *
     * @return int
     */
    public function getAccounttype()
    {
        return $this->get(self::ACCOUNTTYPE);
    }

    /**
     * Sets value of 'time' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTime($value)
    {
        return $this->set(self::TIME, $value);
    }

    /**
     * Returns value of 'time' property
     *
     * @return int
     */
    public function getTime()
    {
        return $this->get(self::TIME);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * LogoutRequest message
 */
class LogoutRequest extends \ProtobufMessage
{
    /* Field index constants */
    const DEVICETYPE = 1;
    const DEVICEID = 2;
    const CONID = 3;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::DEVICETYPE => array(
            'name' => 'deviceType',
            'required' => true,
            'type' => 5,
        ),
        self::DEVICEID => array(
            'name' => 'deviceid',
            'required' => true,
            'type' => 7,
        ),
        self::CONID => array(
            'name' => 'conid',
            'required' => true,
            'type' => 5,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::DEVICETYPE] = null;
        $this->values[self::DEVICEID] = null;
        $this->values[self::CONID] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'deviceType' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setDeviceType($value)
    {
        return $this->set(self::DEVICETYPE, $value);
    }

    /**
     * Returns value of 'deviceType' property
     *
     * @return int
     */
    public function getDeviceType()
    {
        return $this->get(self::DEVICETYPE);
    }

    /**
     * Sets value of 'deviceid' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setDeviceid($value)
    {
        return $this->set(self::DEVICEID, $value);
    }

    /**
     * Returns value of 'deviceid' property
     *
     * @return string
     */
    public function getDeviceid()
    {
        return $this->get(self::DEVICEID);
    }

    /**
     * Sets value of 'conid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setConid($value)
    {
        return $this->set(self::CONID, $value);
    }

    /**
     * Returns value of 'conid' property
     *
     * @return int
     */
    public function getConid()
    {
        return $this->get(self::CONID);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * LogoutResult message
 */
class LogoutResult extends \ProtobufMessage
{
    /* Field index constants */
    const RETBASE = 1;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::RETBASE => array(
            'name' => 'retbase',
            'required' => true,
            'type' => '\Com\Adv\Msg\RetBase'
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::RETBASE] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'retbase' property
     *
     * @param \Com\Adv\Msg\RetBase $value Property value
     *
     * @return null
     */
    public function setRetbase(\Com\Adv\Msg\RetBase $value)
    {
        return $this->set(self::RETBASE, $value);
    }

    /**
     * Returns value of 'retbase' property
     *
     * @return \Com\Adv\Msg\RetBase
     */
    public function getRetbase()
    {
        return $this->get(self::RETBASE);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * TaskNotify message
 */
class TaskNotify extends \ProtobufMessage
{
    /* Field index constants */
    const TASKTYPE = 1;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::TASKTYPE => array(
            'name' => 'tasktype',
            'required' => true,
            'type' => 5,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::TASKTYPE] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'tasktype' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTasktype($value)
    {
        return $this->set(self::TASKTYPE, $value);
    }

    /**
     * Returns value of 'tasktype' property
     *
     * @return int
     */
    public function getTasktype()
    {
        return $this->get(self::TASKTYPE);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * SyncTaskRequest message
 */
class SyncTaskRequest extends \ProtobufMessage
{
    /* Field index constants */
    const SYNCPOINT = 1;
    const SYNCTYPE = 2;
    const SYNCFLAG = 3;
    const SYNCLIMIT = 4;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::SYNCPOINT => array(
            'name' => 'syncpoint',
            'required' => true,
            'type' => 5,
        ),
        self::SYNCTYPE => array(
            'name' => 'synctype',
            'required' => true,
            'type' => 5,
        ),
        self::SYNCFLAG => array(
            'name' => 'syncflag',
            'required' => false,
            'type' => 5,
        ),
        self::SYNCLIMIT => array(
            'name' => 'synclimit',
            'required' => false,
            'type' => 5,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::SYNCPOINT] = null;
        $this->values[self::SYNCTYPE] = null;
        $this->values[self::SYNCFLAG] = null;
        $this->values[self::SYNCLIMIT] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'syncpoint' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setSyncpoint($value)
    {
        return $this->set(self::SYNCPOINT, $value);
    }

    /**
     * Returns value of 'syncpoint' property
     *
     * @return int
     */
    public function getSyncpoint()
    {
        return $this->get(self::SYNCPOINT);
    }

    /**
     * Sets value of 'synctype' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setSynctype($value)
    {
        return $this->set(self::SYNCTYPE, $value);
    }

    /**
     * Returns value of 'synctype' property
     *
     * @return int
     */
    public function getSynctype()
    {
        return $this->get(self::SYNCTYPE);
    }

    /**
     * Sets value of 'syncflag' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setSyncflag($value)
    {
        return $this->set(self::SYNCFLAG, $value);
    }

    /**
     * Returns value of 'syncflag' property
     *
     * @return int
     */
    public function getSyncflag()
    {
        return $this->get(self::SYNCFLAG);
    }

    /**
     * Sets value of 'synclimit' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setSynclimit($value)
    {
        return $this->set(self::SYNCLIMIT, $value);
    }

    /**
     * Returns value of 'synclimit' property
     *
     * @return int
     */
    public function getSynclimit()
    {
        return $this->get(self::SYNCLIMIT);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * SyncTaskResult message
 */
class SyncTaskResult extends \ProtobufMessage
{
    /* Field index constants */
    const RETBASE = 1;
    const TASKINFOS = 2;
    const SYNCTYPE = 3;
    const CONTINUEFLAG = 4;
    const MAXTASKID = 5;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::RETBASE => array(
            'name' => 'retbase',
            'required' => true,
            'type' => '\Com\Adv\Msg\RetBase'
        ),
        self::TASKINFOS => array(
            'name' => 'taskinfos',
            'repeated' => true,
            'type' => '\Com\Adv\Msg\TaskInfo'
        ),
        self::SYNCTYPE => array(
            'name' => 'synctype',
            'required' => true,
            'type' => 5,
        ),
        self::CONTINUEFLAG => array(
            'name' => 'continueflag',
            'required' => false,
            'type' => 5,
        ),
        self::MAXTASKID => array(
            'name' => 'maxtaskid',
            'required' => false,
            'type' => 5,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::RETBASE] = null;
        $this->values[self::TASKINFOS] = array();
        $this->values[self::SYNCTYPE] = null;
        $this->values[self::CONTINUEFLAG] = null;
        $this->values[self::MAXTASKID] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'retbase' property
     *
     * @param \Com\Adv\Msg\RetBase $value Property value
     *
     * @return null
     */
    public function setRetbase(\Com\Adv\Msg\RetBase $value)
    {
        return $this->set(self::RETBASE, $value);
    }

    /**
     * Returns value of 'retbase' property
     *
     * @return \Com\Adv\Msg\RetBase
     */
    public function getRetbase()
    {
        return $this->get(self::RETBASE);
    }

    /**
     * Appends value to 'taskinfos' list
     *
     * @param \Com\Adv\Msg\TaskInfo $value Value to append
     *
     * @return null
     */
    public function appendTaskinfos(\Com\Adv\Msg\TaskInfo $value)
    {
        return $this->append(self::TASKINFOS, $value);
    }

    /**
     * Clears 'taskinfos' list
     *
     * @return null
     */
    public function clearTaskinfos()
    {
        return $this->clear(self::TASKINFOS);
    }

    /**
     * Returns 'taskinfos' list
     *
     * @return \Com\Adv\Msg\TaskInfo[]
     */
    public function getTaskinfos()
    {
        return $this->get(self::TASKINFOS);
    }

    /**
     * Returns 'taskinfos' iterator
     *
     * @return ArrayIterator
     */
    public function getTaskinfosIterator()
    {
        return new \ArrayIterator($this->get(self::TASKINFOS));
    }

    /**
     * Returns element from 'taskinfos' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return \Com\Adv\Msg\TaskInfo
     */
    public function getTaskinfosAt($offset)
    {
        return $this->get(self::TASKINFOS, $offset);
    }

    /**
     * Returns count of 'taskinfos' list
     *
     * @return int
     */
    public function getTaskinfosCount()
    {
        return $this->count(self::TASKINFOS);
    }

    /**
     * Sets value of 'synctype' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setSynctype($value)
    {
        return $this->set(self::SYNCTYPE, $value);
    }

    /**
     * Returns value of 'synctype' property
     *
     * @return int
     */
    public function getSynctype()
    {
        return $this->get(self::SYNCTYPE);
    }

    /**
     * Sets value of 'continueflag' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setContinueflag($value)
    {
        return $this->set(self::CONTINUEFLAG, $value);
    }

    /**
     * Returns value of 'continueflag' property
     *
     * @return int
     */
    public function getContinueflag()
    {
        return $this->get(self::CONTINUEFLAG);
    }

    /**
     * Sets value of 'maxtaskid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setMaxtaskid($value)
    {
        return $this->set(self::MAXTASKID, $value);
    }

    /**
     * Returns value of 'maxtaskid' property
     *
     * @return int
     */
    public function getMaxtaskid()
    {
        return $this->get(self::MAXTASKID);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * ReportTaskRequest message
 */
class ReportTaskRequest extends \ProtobufMessage
{
    /* Field index constants */
    const TASKINFO = 1;
    const REWARD = 2;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::TASKINFO => array(
            'name' => 'taskinfo',
            'required' => true,
            'type' => '\Com\Adv\Msg\TaskInfo'
        ),
        self::REWARD => array(
            'name' => 'reward',
            'required' => false,
            'type' => 7,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::TASKINFO] = null;
        $this->values[self::REWARD] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'taskinfo' property
     *
     * @param \Com\Adv\Msg\TaskInfo $value Property value
     *
     * @return null
     */
    public function setTaskinfo(\Com\Adv\Msg\TaskInfo $value)
    {
        return $this->set(self::TASKINFO, $value);
    }

    /**
     * Returns value of 'taskinfo' property
     *
     * @return \Com\Adv\Msg\TaskInfo
     */
    public function getTaskinfo()
    {
        return $this->get(self::TASKINFO);
    }

    /**
     * Sets value of 'reward' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setReward($value)
    {
        return $this->set(self::REWARD, $value);
    }

    /**
     * Returns value of 'reward' property
     *
     * @return string
     */
    public function getReward()
    {
        return $this->get(self::REWARD);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * ReportTaskResult message
 */
class ReportTaskResult extends \ProtobufMessage
{
    /* Field index constants */
    const RETBASE = 1;
    const TASKID = 2;
    const REWARD = 3;
    const DONE_STEP = 4;
    const TASKTYPE = 5;
    const REPORT_TIME = 6;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::RETBASE => array(
            'name' => 'retbase',
            'required' => true,
            'type' => '\Com\Adv\Msg\RetBase'
        ),
        self::TASKID => array(
            'name' => 'taskid',
            'required' => true,
            'type' => 5,
        ),
        self::REWARD => array(
            'name' => 'reward',
            'required' => true,
            'type' => 7,
        ),
        self::DONE_STEP => array(
            'name' => 'done_step',
            'required' => false,
            'type' => 5,
        ),
        self::TASKTYPE => array(
            'name' => 'tasktype',
            'required' => false,
            'type' => 5,
        ),
        self::REPORT_TIME => array(
            'name' => 'report_time',
            'required' => false,
            'type' => 5,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::RETBASE] = null;
        $this->values[self::TASKID] = null;
        $this->values[self::REWARD] = null;
        $this->values[self::DONE_STEP] = null;
        $this->values[self::TASKTYPE] = null;
        $this->values[self::REPORT_TIME] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'retbase' property
     *
     * @param \Com\Adv\Msg\RetBase $value Property value
     *
     * @return null
     */
    public function setRetbase(\Com\Adv\Msg\RetBase $value)
    {
        return $this->set(self::RETBASE, $value);
    }

    /**
     * Returns value of 'retbase' property
     *
     * @return \Com\Adv\Msg\RetBase
     */
    public function getRetbase()
    {
        return $this->get(self::RETBASE);
    }

    /**
     * Sets value of 'taskid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTaskid($value)
    {
        return $this->set(self::TASKID, $value);
    }

    /**
     * Returns value of 'taskid' property
     *
     * @return int
     */
    public function getTaskid()
    {
        return $this->get(self::TASKID);
    }

    /**
     * Sets value of 'reward' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setReward($value)
    {
        return $this->set(self::REWARD, $value);
    }

    /**
     * Returns value of 'reward' property
     *
     * @return string
     */
    public function getReward()
    {
        return $this->get(self::REWARD);
    }

    /**
     * Sets value of 'done_step' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setDoneStep($value)
    {
        return $this->set(self::DONE_STEP, $value);
    }

    /**
     * Returns value of 'done_step' property
     *
     * @return int
     */
    public function getDoneStep()
    {
        return $this->get(self::DONE_STEP);
    }

    /**
     * Sets value of 'tasktype' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTasktype($value)
    {
        return $this->set(self::TASKTYPE, $value);
    }

    /**
     * Returns value of 'tasktype' property
     *
     * @return int
     */
    public function getTasktype()
    {
        return $this->get(self::TASKTYPE);
    }

    /**
     * Sets value of 'report_time' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setReportTime($value)
    {
        return $this->set(self::REPORT_TIME, $value);
    }

    /**
     * Returns value of 'report_time' property
     *
     * @return int
     */
    public function getReportTime()
    {
        return $this->get(self::REPORT_TIME);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * GetIncomeRequest message
 */
class GetIncomeRequest extends \ProtobufMessage
{
    /* Field index constants */
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * GetIncomeResult message
 */
class GetIncomeResult extends \ProtobufMessage
{
    /* Field index constants */
    const RETBASE = 1;
    const PRE_CASH = 2;
    const USEABLE_CASH = 3;
    const FETCHED_CASH = 4;
    const RANK = 5;
    const DESC = 6;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::RETBASE => array(
            'name' => 'retbase',
            'required' => true,
            'type' => '\Com\Adv\Msg\RetBase'
        ),
        self::PRE_CASH => array(
            'name' => 'pre_cash',
            'required' => true,
            'type' => 7,
        ),
        self::USEABLE_CASH => array(
            'name' => 'useable_cash',
            'required' => true,
            'type' => 7,
        ),
        self::FETCHED_CASH => array(
            'name' => 'fetched_cash',
            'required' => true,
            'type' => 7,
        ),
        self::RANK => array(
            'name' => 'rank',
            'required' => false,
            'type' => 5,
        ),
        self::DESC => array(
            'name' => 'desc',
            'required' => false,
            'type' => 7,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::RETBASE] = null;
        $this->values[self::PRE_CASH] = null;
        $this->values[self::USEABLE_CASH] = null;
        $this->values[self::FETCHED_CASH] = null;
        $this->values[self::RANK] = null;
        $this->values[self::DESC] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'retbase' property
     *
     * @param \Com\Adv\Msg\RetBase $value Property value
     *
     * @return null
     */
    public function setRetbase(\Com\Adv\Msg\RetBase $value)
    {
        return $this->set(self::RETBASE, $value);
    }

    /**
     * Returns value of 'retbase' property
     *
     * @return \Com\Adv\Msg\RetBase
     */
    public function getRetbase()
    {
        return $this->get(self::RETBASE);
    }

    /**
     * Sets value of 'pre_cash' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setPreCash($value)
    {
        return $this->set(self::PRE_CASH, $value);
    }

    /**
     * Returns value of 'pre_cash' property
     *
     * @return string
     */
    public function getPreCash()
    {
        return $this->get(self::PRE_CASH);
    }

    /**
     * Sets value of 'useable_cash' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setUseableCash($value)
    {
        return $this->set(self::USEABLE_CASH, $value);
    }

    /**
     * Returns value of 'useable_cash' property
     *
     * @return string
     */
    public function getUseableCash()
    {
        return $this->get(self::USEABLE_CASH);
    }

    /**
     * Sets value of 'fetched_cash' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setFetchedCash($value)
    {
        return $this->set(self::FETCHED_CASH, $value);
    }

    /**
     * Returns value of 'fetched_cash' property
     *
     * @return string
     */
    public function getFetchedCash()
    {
        return $this->get(self::FETCHED_CASH);
    }

    /**
     * Sets value of 'rank' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setRank($value)
    {
        return $this->set(self::RANK, $value);
    }

    /**
     * Returns value of 'rank' property
     *
     * @return int
     */
    public function getRank()
    {
        return $this->get(self::RANK);
    }

    /**
     * Sets value of 'desc' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setDesc($value)
    {
        return $this->set(self::DESC, $value);
    }

    /**
     * Returns value of 'desc' property
     *
     * @return string
     */
    public function getDesc()
    {
        return $this->get(self::DESC);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * AckRequest message
 */
class AckRequest extends \ProtobufMessage
{
    /* Field index constants */
    const TYPE = 1;
    const ID = 2;
    const TIME = 3;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::TYPE => array(
            'name' => 'type',
            'required' => true,
            'type' => 5,
        ),
        self::ID => array(
            'name' => 'id',
            'repeated' => true,
            'type' => 5,
        ),
        self::TIME => array(
            'name' => 'time',
            'required' => false,
            'type' => 5,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::TYPE] = null;
        $this->values[self::ID] = array();
        $this->values[self::TIME] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'type' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setType($value)
    {
        return $this->set(self::TYPE, $value);
    }

    /**
     * Returns value of 'type' property
     *
     * @return int
     */
    public function getType()
    {
        return $this->get(self::TYPE);
    }

    /**
     * Appends value to 'id' list
     *
     * @param int $value Value to append
     *
     * @return null
     */
    public function appendId($value)
    {
        return $this->append(self::ID, $value);
    }

    /**
     * Clears 'id' list
     *
     * @return null
     */
    public function clearId()
    {
        return $this->clear(self::ID);
    }

    /**
     * Returns 'id' list
     *
     * @return int[]
     */
    public function getId()
    {
        return $this->get(self::ID);
    }

    /**
     * Returns 'id' iterator
     *
     * @return ArrayIterator
     */
    public function getIdIterator()
    {
        return new \ArrayIterator($this->get(self::ID));
    }

    /**
     * Returns element from 'id' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return int
     */
    public function getIdAt($offset)
    {
        return $this->get(self::ID, $offset);
    }

    /**
     * Returns count of 'id' list
     *
     * @return int
     */
    public function getIdCount()
    {
        return $this->count(self::ID);
    }

    /**
     * Sets value of 'time' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTime($value)
    {
        return $this->set(self::TIME, $value);
    }

    /**
     * Returns value of 'time' property
     *
     * @return int
     */
    public function getTime()
    {
        return $this->get(self::TIME);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * SaveTaskRequest message
 */
class SaveTaskRequest extends \ProtobufMessage
{
    /* Field index constants */
    const TASKINFO = 1;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::TASKINFO => array(
            'name' => 'taskinfo',
            'repeated' => true,
            'type' => '\Com\Adv\Msg\TaskInfo'
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::TASKINFO] = array();
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Appends value to 'taskinfo' list
     *
     * @param \Com\Adv\Msg\TaskInfo $value Value to append
     *
     * @return null
     */
    public function appendTaskinfo(\Com\Adv\Msg\TaskInfo $value)
    {
        return $this->append(self::TASKINFO, $value);
    }

    /**
     * Clears 'taskinfo' list
     *
     * @return null
     */
    public function clearTaskinfo()
    {
        return $this->clear(self::TASKINFO);
    }

    /**
     * Returns 'taskinfo' list
     *
     * @return \Com\Adv\Msg\TaskInfo[]
     */
    public function getTaskinfo()
    {
        return $this->get(self::TASKINFO);
    }

    /**
     * Returns 'taskinfo' iterator
     *
     * @return ArrayIterator
     */
    public function getTaskinfoIterator()
    {
        return new \ArrayIterator($this->get(self::TASKINFO));
    }

    /**
     * Returns element from 'taskinfo' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return \Com\Adv\Msg\TaskInfo
     */
    public function getTaskinfoAt($offset)
    {
        return $this->get(self::TASKINFO, $offset);
    }

    /**
     * Returns count of 'taskinfo' list
     *
     * @return int
     */
    public function getTaskinfoCount()
    {
        return $this->count(self::TASKINFO);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * SaveTaskResult message
 */
class SaveTaskResult extends \ProtobufMessage
{
    /* Field index constants */
    const RETBASE = 1;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::RETBASE => array(
            'name' => 'retbase',
            'required' => true,
            'type' => '\Com\Adv\Msg\RetBase'
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::RETBASE] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'retbase' property
     *
     * @param \Com\Adv\Msg\RetBase $value Property value
     *
     * @return null
     */
    public function setRetbase(\Com\Adv\Msg\RetBase $value)
    {
        return $this->set(self::RETBASE, $value);
    }

    /**
     * Returns value of 'retbase' property
     *
     * @return \Com\Adv\Msg\RetBase
     */
    public function getRetbase()
    {
        return $this->get(self::RETBASE);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * MysqlRequest message
 */
class MysqlRequest extends \ProtobufMessage
{
    /* Field index constants */
    const SQLID = 1;
    const SQL = 2;
    const ISREQUESTFIELD = 3;
    const LIMIT = 4;
    const TIMEOUT = 5;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::SQLID => array(
            'name' => 'sqlid',
            'required' => true,
            'type' => 5,
        ),
        self::SQL => array(
            'name' => 'sql',
            'required' => true,
            'type' => 7,
        ),
        self::ISREQUESTFIELD => array(
            'name' => 'isrequestfield',
            'required' => false,
            'type' => 5,
        ),
        self::LIMIT => array(
            'name' => 'limit',
            'required' => false,
            'type' => 5,
        ),
        self::TIMEOUT => array(
            'name' => 'timeout',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::SQLID] = null;
        $this->values[self::SQL] = null;
        $this->values[self::ISREQUESTFIELD] = null;
        $this->values[self::LIMIT] = null;
        $this->values[self::TIMEOUT] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'sqlid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setSqlid($value)
    {
        return $this->set(self::SQLID, $value);
    }

    /**
     * Returns value of 'sqlid' property
     *
     * @return int
     */
    public function getSqlid()
    {
        return $this->get(self::SQLID);
    }

    /**
     * Sets value of 'sql' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setSql($value)
    {
        return $this->set(self::SQL, $value);
    }

    /**
     * Returns value of 'sql' property
     *
     * @return string
     */
    public function getSql()
    {
        return $this->get(self::SQL);
    }

    /**
     * Sets value of 'isrequestfield' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setIsrequestfield($value)
    {
        return $this->set(self::ISREQUESTFIELD, $value);
    }

    /**
     * Returns value of 'isrequestfield' property
     *
     * @return int
     */
    public function getIsrequestfield()
    {
        return $this->get(self::ISREQUESTFIELD);
    }

    /**
     * Sets value of 'limit' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setLimit($value)
    {
        return $this->set(self::LIMIT, $value);
    }

    /**
     * Returns value of 'limit' property
     *
     * @return int
     */
    public function getLimit()
    {
        return $this->get(self::LIMIT);
    }

    /**
     * Sets value of 'timeout' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTimeout($value)
    {
        return $this->set(self::TIMEOUT, $value);
    }

    /**
     * Returns value of 'timeout' property
     *
     * @return int
     */
    public function getTimeout()
    {
        return $this->get(self::TIMEOUT);
    }
}
}

namespace Com\Adv\Msg {
/**
 * MysqlResult message
 */
class MysqlResult extends \ProtobufMessage
{
    /* Field index constants */
    const RETBASE = 1;
    const AFFECTEDROWS = 2;
    const INSERTID = 3;
    const ROWS = 4;
    const SQLID = 5;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::RETBASE => array(
            'name' => 'retbase',
            'required' => true,
            'type' => '\Com\Adv\Msg\RetBase'
        ),
        self::AFFECTEDROWS => array(
            'name' => 'affectedrows',
            'required' => false,
            'type' => 5,
        ),
        self::INSERTID => array(
            'name' => 'insertid',
            'required' => false,
            'type' => 5,
        ),
        self::ROWS => array(
            'name' => 'rows',
            'repeated' => true,
            'type' => '\Com\Adv\Msg\PRow'
        ),
        self::SQLID => array(
            'name' => 'sqlid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::RETBASE] = null;
        $this->values[self::AFFECTEDROWS] = null;
        $this->values[self::INSERTID] = null;
        $this->values[self::ROWS] = array();
        $this->values[self::SQLID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'retbase' property
     *
     * @param \Com\Adv\Msg\RetBase $value Property value
     *
     * @return null
     */
    public function setRetbase(\Com\Adv\Msg\RetBase $value)
    {
        return $this->set(self::RETBASE, $value);
    }

    /**
     * Returns value of 'retbase' property
     *
     * @return \Com\Adv\Msg\RetBase
     */
    public function getRetbase()
    {
        return $this->get(self::RETBASE);
    }

    /**
     * Sets value of 'affectedrows' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setAffectedrows($value)
    {
        return $this->set(self::AFFECTEDROWS, $value);
    }

    /**
     * Returns value of 'affectedrows' property
     *
     * @return int
     */
    public function getAffectedrows()
    {
        return $this->get(self::AFFECTEDROWS);
    }

    /**
     * Sets value of 'insertid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setInsertid($value)
    {
        return $this->set(self::INSERTID, $value);
    }

    /**
     * Returns value of 'insertid' property
     *
     * @return int
     */
    public function getInsertid()
    {
        return $this->get(self::INSERTID);
    }

    /**
     * Appends value to 'rows' list
     *
     * @param \Com\Adv\Msg\PRow $value Value to append
     *
     * @return null
     */
    public function appendRows(\Com\Adv\Msg\PRow $value)
    {
        return $this->append(self::ROWS, $value);
    }

    /**
     * Clears 'rows' list
     *
     * @return null
     */
    public function clearRows()
    {
        return $this->clear(self::ROWS);
    }

    /**
     * Returns 'rows' list
     *
     * @return \Com\Adv\Msg\PRow[]
     */
    public function getRows()
    {
        return $this->get(self::ROWS);
    }

    /**
     * Returns 'rows' iterator
     *
     * @return ArrayIterator
     */
    public function getRowsIterator()
    {
        return new \ArrayIterator($this->get(self::ROWS));
    }

    /**
     * Returns element from 'rows' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return \Com\Adv\Msg\PRow
     */
    public function getRowsAt($offset)
    {
        return $this->get(self::ROWS, $offset);
    }

    /**
     * Returns count of 'rows' list
     *
     * @return int
     */
    public function getRowsCount()
    {
        return $this->count(self::ROWS);
    }

    /**
     * Sets value of 'sqlid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setSqlid($value)
    {
        return $this->set(self::SQLID, $value);
    }

    /**
     * Returns value of 'sqlid' property
     *
     * @return int
     */
    public function getSqlid()
    {
        return $this->get(self::SQLID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * RedispRequest message
 */
class RedispRequest extends \ProtobufMessage
{
    /* Field index constants */
    const SQLID = 1;
    const KEY = 2;
    const PARAM = 3;
    const LIMIT = 4;
    const TIMEOUT = 5;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::SQLID => array(
            'name' => 'sqlid',
            'required' => true,
            'type' => 5,
        ),
        self::KEY => array(
            'name' => 'key',
            'required' => true,
            'type' => 7,
        ),
        self::PARAM => array(
            'name' => 'param',
            'required' => false,
            'type' => 5,
        ),
        self::LIMIT => array(
            'name' => 'limit',
            'required' => false,
            'type' => 5,
        ),
        self::TIMEOUT => array(
            'name' => 'timeout',
            'required' => false,
            'type' => 5,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::SQLID] = null;
        $this->values[self::KEY] = null;
        $this->values[self::PARAM] = null;
        $this->values[self::LIMIT] = null;
        $this->values[self::TIMEOUT] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'sqlid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setSqlid($value)
    {
        return $this->set(self::SQLID, $value);
    }

    /**
     * Returns value of 'sqlid' property
     *
     * @return int
     */
    public function getSqlid()
    {
        return $this->get(self::SQLID);
    }

    /**
     * Sets value of 'key' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setKey($value)
    {
        return $this->set(self::KEY, $value);
    }

    /**
     * Returns value of 'key' property
     *
     * @return string
     */
    public function getKey()
    {
        return $this->get(self::KEY);
    }

    /**
     * Sets value of 'param' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setParam($value)
    {
        return $this->set(self::PARAM, $value);
    }

    /**
     * Returns value of 'param' property
     *
     * @return int
     */
    public function getParam()
    {
        return $this->get(self::PARAM);
    }

    /**
     * Sets value of 'limit' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setLimit($value)
    {
        return $this->set(self::LIMIT, $value);
    }

    /**
     * Returns value of 'limit' property
     *
     * @return int
     */
    public function getLimit()
    {
        return $this->get(self::LIMIT);
    }

    /**
     * Sets value of 'timeout' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTimeout($value)
    {
        return $this->set(self::TIMEOUT, $value);
    }

    /**
     * Returns value of 'timeout' property
     *
     * @return int
     */
    public function getTimeout()
    {
        return $this->get(self::TIMEOUT);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * RedispResult message
 */
class RedispResult extends \ProtobufMessage
{
    /* Field index constants */
    const RETBASE = 1;
    const AFFECTEDROWS = 2;
    const INSERTID = 3;
    const ROWS = 4;
    const SQLID = 5;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::RETBASE => array(
            'name' => 'retbase',
            'required' => true,
            'type' => '\Com\Adv\Msg\RetBase'
        ),
        self::AFFECTEDROWS => array(
            'name' => 'affectedrows',
            'required' => false,
            'type' => 5,
        ),
        self::INSERTID => array(
            'name' => 'insertid',
            'required' => false,
            'type' => 5,
        ),
        self::ROWS => array(
            'name' => 'rows',
            'repeated' => true,
            'type' => '\Com\Adv\Msg\PRow'
        ),
        self::SQLID => array(
            'name' => 'sqlid',
            'required' => false,
            'type' => 5,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::RETBASE] = null;
        $this->values[self::AFFECTEDROWS] = null;
        $this->values[self::INSERTID] = null;
        $this->values[self::ROWS] = array();
        $this->values[self::SQLID] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'retbase' property
     *
     * @param \Com\Adv\Msg\RetBase $value Property value
     *
     * @return null
     */
    public function setRetbase(\Com\Adv\Msg\RetBase $value)
    {
        return $this->set(self::RETBASE, $value);
    }

    /**
     * Returns value of 'retbase' property
     *
     * @return \Com\Adv\Msg\RetBase
     */
    public function getRetbase()
    {
        return $this->get(self::RETBASE);
    }

    /**
     * Sets value of 'affectedrows' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setAffectedrows($value)
    {
        return $this->set(self::AFFECTEDROWS, $value);
    }

    /**
     * Returns value of 'affectedrows' property
     *
     * @return int
     */
    public function getAffectedrows()
    {
        return $this->get(self::AFFECTEDROWS);
    }

    /**
     * Sets value of 'insertid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setInsertid($value)
    {
        return $this->set(self::INSERTID, $value);
    }

    /**
     * Returns value of 'insertid' property
     *
     * @return int
     */
    public function getInsertid()
    {
        return $this->get(self::INSERTID);
    }

    /**
     * Appends value to 'rows' list
     *
     * @param \Com\Adv\Msg\PRow $value Value to append
     *
     * @return null
     */
    public function appendRows(\Com\Adv\Msg\PRow $value)
    {
        return $this->append(self::ROWS, $value);
    }

    /**
     * Clears 'rows' list
     *
     * @return null
     */
    public function clearRows()
    {
        return $this->clear(self::ROWS);
    }

    /**
     * Returns 'rows' list
     *
     * @return \Com\Adv\Msg\PRow[]
     */
    public function getRows()
    {
        return $this->get(self::ROWS);
    }

    /**
     * Returns 'rows' iterator
     *
     * @return ArrayIterator
     */
    public function getRowsIterator()
    {
        return new \ArrayIterator($this->get(self::ROWS));
    }

    /**
     * Returns element from 'rows' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return \Com\Adv\Msg\PRow
     */
    public function getRowsAt($offset)
    {
        return $this->get(self::ROWS, $offset);
    }

    /**
     * Returns count of 'rows' list
     *
     * @return int
     */
    public function getRowsCount()
    {
        return $this->count(self::ROWS);
    }

    /**
     * Sets value of 'sqlid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setSqlid($value)
    {
        return $this->set(self::SQLID, $value);
    }

    /**
     * Returns value of 'sqlid' property
     *
     * @return int
     */
    public function getSqlid()
    {
        return $this->get(self::SQLID);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}

namespace Com\Adv\Msg {
/**
 * InnerCommonMsg message
 */
class InnerCommonMsg extends \ProtobufMessage
{
    /* Field index constants */
    const ITEMS = 1;
    const TRANSFER = 2;
    const TIME = 3;
    const TRANSID = 32;

    /* @var array Field descriptors */
    protected static $fields = array(
        self::ITEMS => array(
            'name' => 'items',
            'repeated' => true,
            'type' => '\Com\Adv\Msg\PDataCell'
        ),
        self::TRANSFER => array(
            'name' => 'transfer',
            'required' => false,
            'type' => 7,
        ),
        self::TIME => array(
            'name' => 'time',
            'required' => false,
            'type' => 5,
        ),
        self::TRANSID => array(
            'name' => 'transid',
            'required' => false,
            'type' => 5,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::ITEMS] = array();
        $this->values[self::TRANSFER] = null;
        $this->values[self::TIME] = null;
        $this->values[self::TRANSID] = null;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Appends value to 'items' list
     *
     * @param \Com\Adv\Msg\PDataCell $value Value to append
     *
     * @return null
     */
    public function appendItems(\Com\Adv\Msg\PDataCell $value)
    {
        return $this->append(self::ITEMS, $value);
    }

    /**
     * Clears 'items' list
     *
     * @return null
     */
    public function clearItems()
    {
        return $this->clear(self::ITEMS);
    }

    /**
     * Returns 'items' list
     *
     * @return \Com\Adv\Msg\PDataCell[]
     */
    public function getItems()
    {
        return $this->get(self::ITEMS);
    }

    /**
     * Returns 'items' iterator
     *
     * @return ArrayIterator
     */
    public function getItemsIterator()
    {
        return new \ArrayIterator($this->get(self::ITEMS));
    }

    /**
     * Returns element from 'items' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return \Com\Adv\Msg\PDataCell
     */
    public function getItemsAt($offset)
    {
        return $this->get(self::ITEMS, $offset);
    }

    /**
     * Returns count of 'items' list
     *
     * @return int
     */
    public function getItemsCount()
    {
        return $this->count(self::ITEMS);
    }

    /**
     * Sets value of 'transfer' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function setTransfer($value)
    {
        return $this->set(self::TRANSFER, $value);
    }

    /**
     * Returns value of 'transfer' property
     *
     * @return string
     */
    public function getTransfer()
    {
        return $this->get(self::TRANSFER);
    }

    /**
     * Sets value of 'time' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTime($value)
    {
        return $this->set(self::TIME, $value);
    }

    /**
     * Returns value of 'time' property
     *
     * @return int
     */
    public function getTime()
    {
        return $this->get(self::TIME);
    }

    /**
     * Sets value of 'transid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function setTransid($value)
    {
        return $this->set(self::TRANSID, $value);
    }

    /**
     * Returns value of 'transid' property
     *
     * @return int
     */
    public function getTransid()
    {
        return $this->get(self::TRANSID);
    }
}
}
namespace {
require_once 'pb_proto_adv_base.php';
}