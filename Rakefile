require "bundler"
require "pathname"
Bundler.setup

CURRENT_DIR = Pathname.new(Dir.pwd)
CEEDLING_PATH = Pathname.new(Gem.loaded_specs["ceedling"].gem_dir).relative_path_from(CURRENT_DIR).to_s

require "ceedling"
Ceedling.load_project
