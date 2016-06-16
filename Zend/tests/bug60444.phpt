--TEST--
Bug #60444 (Segmentation fault with include & class extending)
--FILE--
<?php

class Foo
{
    public function __construct()
    {
        eval("class Bar extends Foo {}");
        Some::foo($this);
    }
}
class Some
{
    public static function foo(Foo $foo)
    {
    }
}
function fn43954418()
{
    new Foo();
    echo "done\n";
}
fn43954418();
--EXPECT--
done
