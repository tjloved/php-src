--TEST--
Variable as class name for new expression
--FILE--
<?php

class Test
{
    public static $className = 'stdClass';
}
function fn2131790149()
{
    $className = 'stdClass';
    $array = ['className' => 'stdClass'];
    $obj = (object) ['className' => 'stdClass'];
    $test = 'Test';
    $weird = [0 => (object) ['foo' => 'Test']];
    var_dump(new $className());
    var_dump(new $array['className']());
    var_dump(new $array['className']());
    var_dump(new $obj->className());
    var_dump(new Test::$className());
    var_dump(new $test::$className());
    var_dump(new $weird[0]->foo::$className());
}
fn2131790149();
--EXPECTF--
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
