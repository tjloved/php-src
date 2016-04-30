--TEST--
Don't inline if late static binding is used
--FILE--
<?php

class A {
    public static function who() {
        echo "A\n";
    }

    public static function call_who() {
        static::who();
    }

    public static function get_called_class() {
        echo get_called_class(), "\n";
    }

    public static function test() {
        A::call_who();
        B::call_who();
        A::get_called_class();
        B::get_called_class();
    }
}

class B extends A {
    public static function who() {
        echo "B\n";
    }
}

A::test();

?>
--EXPECT--
A
B
A
B
