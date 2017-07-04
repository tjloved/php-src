--TEST--
Bug #61970 (Restraining __construct() access level in subclass gives a fatal error)
--FILE--
<?php

class Foo
{
    public function __construct()
    {
    }
}
class Bar extends Foo
{
    protected function __construct()
    {
    }
}
function fn456249746()
{
    echo 'DONE';
}
fn456249746();
--EXPECT--
DONE
