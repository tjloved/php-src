--TEST--
Bug #65108 (is_callable() triggers Fatal Error)
--FILE--
<?php

class C
{
    private function f()
    {
    }
    static function __callStatic($name, $args)
    {
    }
}
class B
{
    public function __construct()
    {
        $isCallable = is_callable(array(new C(), 'f'));
        var_dump($isCallable);
    }
}
class E
{
    private function f()
    {
    }
    function __call($name, $args)
    {
    }
}
function fn884508990()
{
    new B();
    $isCallable = is_callable(array('E', 'f'));
    var_dump($isCallable);
}
fn884508990();
--EXPECT--
bool(false)
bool(false)
