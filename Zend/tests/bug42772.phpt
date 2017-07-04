--TEST--
Bug #42772 (Storing $this in a static var fails while handling a cast to string)
--FILE--
<?php

class Foo
{
    public static $foo;
    function __toString()
    {
        self::$foo = $this;
        return 'foo';
    }
}
function fn309447051()
{
    $foo = (string) new Foo();
    var_dump(Foo::$foo);
}
fn309447051();
--EXPECT--
object(Foo)#1 (0) {
}
