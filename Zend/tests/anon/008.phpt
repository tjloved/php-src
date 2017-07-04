--TEST--
testing static access for methods and properties in anon classes
--FILE--
<?php

function fn349741400()
{
    $anonClass = new class("cats", "dogs")
    {
        public static $foo;
        private static $bar;
        public function __construct($foo, $bar)
        {
            static::$foo = $foo;
            static::$bar = $bar;
        }
        public static function getBar()
        {
            return static::$bar;
        }
    };
    var_dump($anonClass::$foo);
    var_dump($anonClass::getBar());
}
fn349741400();
--EXPECT--
string(4) "cats"
string(4) "dogs"
