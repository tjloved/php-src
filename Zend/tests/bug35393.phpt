--TEST--
Bug #35393 (changing static protected members from outside the class)
--INI--
error_reporting=4095
--FILE--
<?php

class ObjectPath
{
    protected static $type = array(0 => 'main');
    static function getType()
    {
        return self::$type;
    }
}
function fn1030578435()
{
    print_r(ObjectPath::getType());
    $object_type = array_pop(ObjectPath::getType());
    print_r(ObjectPath::getType());
}
fn1030578435();
--EXPECTF--
Array
(
    [0] => main
)

Notice: Only variables should be passed by reference in %sbug35393.php on line %d
Array
(
    [0] => main
)
