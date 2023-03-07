BoolFlagProvider = provider(fields = ['value'])

def _impl(ctx):
  return BoolFlagProvider(value = ctx.build_setting_value)

_bool_flag = rule(
    implementation = _impl,
    build_setting = config.bool(flag = True),
)

def optional_setting(name, *, flag_name, visibility):
  _bool_flag(
      name = flag_name,
      build_setting_default = False,
      visibility = visibility,
  )
  native.config_setting(
      name = name,
      flag_values = {
          ":" + flag_name: "true",
      },
      visibility = visibility,
  )
