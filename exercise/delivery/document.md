把其他dll的对象传递到其他dll中，并且不依赖加载顺序。
自己保证依赖的对象已经创建且已经添加到管理器中。
可以在类型构造函数中添加注册函数，在后续使用时再具体获取或者有其他函数来通知所有插件插件已经加载完成。
只是提供一个模板参考，实际使用是给一个qt的三方库配套的。qt能否用不确定。



获取注册的对象

typename result = GetObjectManager().GetObject<type_name>( );

注册对象

GetObjectManager().ObjectRegister(object);

取消注册

GetObjectManager().ObjectUnregister(object);