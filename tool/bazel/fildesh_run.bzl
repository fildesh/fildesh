
def _write_script(ctx):
  script_file = ctx.actions.declare_file(ctx.label.name + ".fildesh")
  src_preamble = ""
  for k in ctx.attr.output_names + ctx.attr.input_names:
    src_preamble += "(: " + k + " Filename .self.opt." + k + ")\n"
  ctx.actions.write(
      output = script_file,
      content = src_preamble + ctx.attr.src_content,
  )
  return script_file


def _run_script(ctx, script_file):
  args = ctx.actions.args()
  for (k, v) in zip(ctx.attr.tool_aliases, ctx.files.tools):
    args.add("-alias")
    args.add_joined([k, v], join_with = "=")
  args.add("-f")
  args.add(script_file)

  option_names = (
    ctx.attr.output_options +
    ctx.attr.output_names +
    ctx.attr.input_options +
    ctx.attr.input_names)
  for (k, v) in zip(option_names, ctx.outputs.outs + ctx.files.inputs):
    args.add_joined(["--" + k, v], join_with = "=")

  ctx.actions.run(
      executable = ctx.executable._fildesh,
      arguments = [args],
      inputs = [script_file] + ctx.files.inputs,
      outputs = ctx.outputs.outs,
      tools = ctx.files.tools,
  )


def _fildesh_run_impl(ctx):
  script_file = _write_script(ctx)
  _run_script(ctx, script_file)

  return DefaultInfo(
      files = depset(ctx.outputs.outs),
      runfiles = ctx.runfiles(files = [script_file] + ctx.outputs.outs),
  )

_fildesh_run_rule = rule(
    implementation = _fildesh_run_impl,
    attrs = {
        "src_content": attr.string(),
        "output_options": attr.string_list(),
        "output_names": attr.string_list(),
        "input_options": attr.string_list(),
        "input_names": attr.string_list(),
        "tool_aliases": attr.string_list(),
        "inputs": attr.label_list(allow_files = True),
        "outs": attr.output_list(),
        "tools": attr.label_list(cfg = "exec"),
        "_fildesh": attr.label(
            default = Label("//tool:fildesh"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
    },
)

def fildesh_run(
    name,
    src_content,
    output_by_option=None,
    output_by_xof=None,
    input_by_option=None,
    input_by_xof=None,
    tool_by_alias=None,
    **kwargs):
  if not (output_by_option or output_by_xof):
    fail("Specify output_by_option or output_by_xof.")
  output_options = sorted(output_by_option or dict())
  output_names = sorted(output_by_xof or dict())
  input_options = sorted(input_by_option or dict())
  input_names = sorted(input_by_xof or dict())
  tool_aliases = sorted(tool_by_alias or dict())
  _fildesh_run_rule(
      name = name,
      src_content = src_content,
      output_options = output_options,
      output_names = output_names,
      input_options = input_options,
      input_names = input_names,
      tool_aliases = tool_aliases,
      outs = [
        output_by_option[k] for k in output_options
      ] + [
        output_by_xof[k] for k in output_names
      ],
      inputs = [
        input_by_option[k] for k in input_options
      ] + [
        input_by_xof[k] for k in input_names
      ],
      tools = [tool_by_alias[k] for k in tool_aliases],
      **kwargs)
