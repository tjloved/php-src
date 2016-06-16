--TEST--
Bug #54910 (Crash when calling call_user_func with unknown function name)
--FILE--
<?php

class A
{
    public function __call($method, $args)
    {
        if (stripos($method, 'get') === 0) {
            return $this->get();
        }
        die("No such method - '{$method}'\n");
    }
    protected function get()
    {
        $class = get_class($this);
        $call = array($class, 'noSuchMethod');
        if (is_callable($call)) {
            call_user_func($call);
        }
    }
}
class B extends A
{
}
function fn2144862006()
{
    $input = new B();
    echo $input->getEmail();
}
fn2144862006();
--EXPECT--
No such method - 'noSuchMethod'
