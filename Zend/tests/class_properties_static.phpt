--TEST--
Static Class Property Expressions
--FILE--
<?php

class Foo
{
    public $b1 = 1 + 1;
    public $b2 = 1 << 2;
    public $b3 = "foo " . " bar " . " baz";
}
function fn415510523()
{
    $f = new Foo();
    var_dump($f->b1, $f->b2, $f->b3);
}
fn415510523();
--EXPECT--
int(2)
int(4)
string(13) "foo  bar  baz"
