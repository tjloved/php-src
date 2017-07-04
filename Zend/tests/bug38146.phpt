--TEST--
Bug #38146 (Cannot use array returned from foo::__get('bar') in write context)
--FILE--
<?php

class foo
{
    public function __get($member)
    {
        $f = array("foo" => "bar", "bar" => "foo");
        return $f;
    }
}
function fn331957334()
{
    $f = new foo();
    foreach ($f->bar as $key => $value) {
        print "{$key} => {$value}\n";
    }
}
fn331957334();
--EXPECT--
foo => bar
bar => foo
