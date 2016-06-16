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
function fn2083055100()
{
    $foo = (string) new Foo();
    var_dump(Foo::$foo);
}
fn2083055100();
--EXPECT--
object(Foo)#1 (0) {
}
