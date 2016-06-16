--TEST--
Bug #55578 (Segfault on implode/concat)
--FILE--
<?php

class Foo
{
    public function __toString()
    {
        return 'Foo';
    }
}
function test($options, $queryPart)
{
    return '' . (0 ? 1 : $queryPart);
}
function fn285972283()
{
    $options = array();
    var_dump(test($options, new Foo()));
}
fn285972283();
--EXPECT--
string(3) "Foo"
